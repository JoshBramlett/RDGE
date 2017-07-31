#pragma once

#include <atomic>
#include <functional>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>

#if defined(__GNUC__) && defined(__x86_64__)
#define PAUSE() \
    __asm__ __volatile__ ("pause");
#else
#define PAUSE()
#endif

#if defined(__APPLE__)
#include <pthread.h>
#define YIELD pthread_yield_np
#elif defined(__linux__)
#include <sched.h>
#define YIELD sched_yield
#else
#error "Unsupported platform"
#endif

#define test_lt(a, b) (0 < ((b)-(a)))
#define test_le(a, b) (0 <= ((b)-(a)))

namespace disruptor {

template<typename T>
struct element_wrapper
{
    T element;
    uint8_t is_eof = 0;
};

/**************************************************************
 *                        sequence
 * ************************************************************/

static constexpr size_t CACHE_LINE_SIZE = 64;
using sequence_id = int_fast64_t; // must be a signed type

#if defined(__GNUC__) && defined(__x86_64__)

// Optimized for target compiler and hardware
struct sequence
{
    sequence (sequence_id val = 0)
        : value(val)
    { }

    sequence_id get (void) const
    {
        __asm__ __volatile__ ("lfence" ::: "memory");
        return value;
    }

    void set (sequence_id val)
    {
        value = val;
        __asm__ __volatile__ ("sfence" ::: "memory");
    }

    sequence_id increment (void)
    {
        return __sync_add_and_fetch(&value, static_cast<sequence_id>(1));
    }

    volatile sequence_id value;
    char /* __attribute__((unused)) */ pad[CACHE_LINE_SIZE - sizeof(sequence_id)];
} __attribute__((__aligned__(CACHE_LINE_SIZE)));

#else

// STL fallback
struct sequence
{
    sequence (sequence_id val = 0)
        : value(val)
    { }

    sequence_id get (void) const
    {
        return value.load(std::memory_order_acquire);
    }

    void set (sequence_id val)
    {
        value.store(val, std::memory_order_release);
    }

    sequence_id increment (void)
    {
        return value.fetch_add(static_cast<sequence_id>(1), std::memory_order_release) + 1;
    }

    [[gnu::unused]] char padl[CACHE_LINE_SIZE - sizeof(std::atomic<sequence_id>)];
    std::atomic<sequence_id> value;
    [[gnu::unused]] char padr[CACHE_LINE_SIZE - sizeof(std::atomic<sequence_id>)];
};

#endif

/**************************************************************
 *                        yield strategy
 * ************************************************************/

// http://www.1024cores.net/home/lock-free-algorithms/tricks/spinning
using yield_strategy = std::function<void(uint32_t&)>;

static yield_strategy ys_active_spin = [](uint32_t&) {
    PAUSE();
};

static yield_strategy ys_passive_spin = [](uint32_t&) {
    YIELD();
};

static yield_strategy ys_hybrid_spin = [](uint32_t& counter) {
    if (counter < 10)
    {
        PAUSE();
    }
    else if (counter < 20)
    {
        for (int i = 0; i < 50; ++i)
        {
            PAUSE();
        }
    }
    else if (counter < 22)
    {
        YIELD();
    }
    else if (counter < 24)
    {
        usleep(0);
    }
    else if (counter < 26)
    {
        usleep(1);
    }
    else
    {
        usleep(10);
    }

    counter++;
};

/**************************************************************
 *                    producer/consumer
 * ************************************************************/

static constexpr size_t DEFAULT_BUFFER_SIZE = 32768; // 2 ^ 15

template<typename T, size_t N = DEFAULT_BUFFER_SIZE>
struct ring_buffer;

template<typename T>
struct producer
{
    using value_type = T;

    explicit producer (std::string name, yield_strategy ys, bool single_threaded = false);

    T& claim (void);
    void publish (void);
    void eof (void);

    //void skip (void);
    //void flush (void);

    ring_buffer<T>* buffer = nullptr;
    uint32_t        index = 0;          // ring_buffer's index of this producer

    std::string name;
    sequence_id last_produced_id = 0;
    sequence_id last_claimed_id = 0;

    struct statistics
    {
        uint64_t claims = 0;
        uint64_t publishes = 0;
        uint64_t yields = 0;
    } stats;

    std::string print_stats (void)
    {
        std::ostringstream ss;
        ss << "producer[" << this->name << "]"
           << " claims=" << this->stats.claims
           << " publishes=" << this->stats.publishes
           << " yields=" << this->stats.yields;

        return ss.str();
    }

private:
    using claim_strategy = std::function<void(void)>;
    using publish_strategy = std::function<void(sequence_id)>;

    void wait_for_slot (void);

    claim_strategy   m_claim;
    publish_strategy m_publish;
    yield_strategy   m_yield;
};

template<typename T>
struct consumer
{
    using value_type = T;

    explicit consumer (std::string name, yield_strategy ys)
        : name(std::move(name))
        , m_yield(ys)
    { }

    ring_buffer<T>* buffer = nullptr;
    uint32_t        index = 0;
    uint32_t        eof_count = 0;

    bool is_working (void) const;
    T& next (void);

    std::string name;
    sequence    cursor;
    sequence_id last_available_id = 0;
    sequence_id current_id = 0;

    struct statistics
    {
        uint64_t consumed = 0;
        uint64_t yields = 0;
    } stats;

    std::string print_stats (void)
    {
        std::ostringstream ss;
        ss << "consumer[" << this->name << "]"
           << " consumed=" << this->stats.consumed
           << " yields=" << this->stats.yields;

        return ss.str();
    }

private:
    element_wrapper<T>& next_internal (void);

    yield_strategy m_yield;
};

template<typename T, size_t N>
struct ring_buffer
{
    static_assert(N != 0 && (N & (~N + 1)) == N, "Size must be a power of two");

    using value_type = T;

    ring_buffer (std::string name)
        : name(std::move(name))
    {
        m_minCursor = [this](void) { // single consumer
            return this->consumers[0]->cursor.get();
        };
    }

    T& at (sequence_id seq)
    {
        return this->ring[seq & mask].element;
    }

    element_wrapper<T>& element_at (sequence_id seq)
    {
        return this->ring[seq & mask];
    }

    size_t index_of (sequence_id seq)
    {
        return seq & mask;
    }

    size_t size (void) const
    {
        return N;
    }

    void add_producer (std::shared_ptr<producer<T>> p)
    {
        p->buffer = this;
        p->index = producers.size();
        this->producers.emplace_back(std::move(p));
    }

    void add_consumer (std::shared_ptr<consumer<T>> c)
    {
        c->buffer = this;
        c->index = consumers.size();
        this->consumers.emplace_back(std::move(c));

        if (consumers.size() == 2)
        {
            throw "more support must be added for multiple consumers";
            //m_minCursor = [this](void) { // multiple consumers
                //sequence_id minimum = this->consumers[0]->cursor.get();
                //auto size = this->consumers.size();
                //for (decltype(size) i = 1; i < size; ++i)
                //{
                    //sequence_id id = this->consumers[i]->cursor.get();
                    //if (0 < (minimum - id))
                    //{
                        //minimum = id;
                    //}
                //}

                //return minimum;
            //};
        }
    }

    sequence_id get_minimum_cursor (void)
    {
        return m_minCursor();
    }

    static constexpr sequence_id mask = N - 1;

    element_wrapper<T> ring[N];

    std::string name;
    sequence_id last_consumed_id = 0;
    sequence    last_claimed_id;
    sequence    cursor;

    std::vector<std::shared_ptr<producer<T>>> producers;
    std::vector<std::shared_ptr<consumer<T>>> consumers;

private:
    using minimum_strategy = std::function<sequence_id(void)>;

    minimum_strategy m_minCursor;
};

template <typename T>
inline
producer<T>::producer (std::string name, yield_strategy ys, bool single_threaded)
    : name(std::move(name))
    , m_yield(ys)
{
    if (single_threaded)
    {
        m_claim = [this](void) {
            this->last_claimed_id++;
            wait_for_slot();
        };

        m_publish = [this](sequence_id id) {
            this->buffer->cursor.set(id);
        };
    }
    else
    {
        m_claim = [this](void) {
            this->last_claimed_id = this->buffer->last_claimed_id.increment();
            this->last_produced_id = this->last_claimed_id - 1;
            wait_for_slot();
        };

        m_publish = [this](sequence_id id) {
            sequence_id expected = id - 1;
            sequence_id current = this->buffer->cursor.get();

            uint32_t yield_count = 0;
            while (test_lt(current, expected))
            {
                this->stats.yields++;
                m_yield(yield_count);
                current = this->buffer->cursor.get();
            }

            this->buffer->cursor.set(id);
        };
    }
}

template <typename T>
inline T&
producer<T>::claim (void)
{
    assert(m_claim != nullptr);

    if (this->last_produced_id == this->last_claimed_id)
    {
        this->stats.claims++;
        m_claim();
    }

    this->last_produced_id++;
    return this->buffer->at(this->last_produced_id);
}

template <typename T>
inline void
producer<T>::publish (void)
{
    assert(m_publish != nullptr);

    if (this->last_produced_id == this->last_claimed_id)
    {
        this->stats.publishes++;
        m_publish(this->last_claimed_id);
    }
}

template <typename T>
inline void
producer<T>::eof (void)
{
    if (this->last_produced_id == this->last_claimed_id)
    {
        this->stats.claims++;
        m_claim();
    }

    this->last_produced_id++;
    this->buffer->element_at(this->last_produced_id).is_eof = 1;

    publish();
}

template <typename T>
inline void
producer<T>::wait_for_slot (void)
{
    assert(m_yield != nullptr);

    sequence_id wrapped = this->last_claimed_id - this->buffer->size();
    if (test_lt(this->buffer->last_consumed_id, wrapped))
    {
        sequence_id minimum = this->buffer->get_minimum_cursor();
        uint32_t yield_count = 0;
        while (test_lt(minimum, wrapped))
        {
            this->stats.yields++;
            m_yield(yield_count);
            minimum = this->buffer->get_minimum_cursor();
        }

        this->buffer->last_consumed_id = minimum;
    }
}

template <typename T>
inline bool
consumer<T>::is_working (void) const
{
    return this->buffer->producers.size() != this->eof_count;
}

template <typename T>
inline T&
consumer<T>::next (void)
{
    do {
        this->stats.consumed++;
        auto& wrapper = next_internal();
        if (wrapper.is_eof != 0)
        {
            this->eof_count++;
            wrapper.is_eof = 0; // flag must be reset

            if (!is_working())
            {
                this->cursor.set(this->current_id);
                throw "eof";
            }
        }
        else
        {
            return wrapper.element;
        }
    } while (true);
}

template <typename T>
inline element_wrapper<T>&
consumer<T>::next_internal (void)
{
    sequence_id last_consumed_id = this->current_id++;
    if (test_le(this->current_id, this->last_available_id))
    {
        return this->buffer->element_at(this->current_id);
    }

    this->cursor.set(last_consumed_id);

    sequence_id buffer_cursor = this->buffer->cursor.get();
    uint32_t yield_count = 0;
    while (test_le(buffer_cursor, last_consumed_id))
    {
        this->stats.yields++;
        m_yield(yield_count);
        buffer_cursor = this->buffer->cursor.get();
    }

    this->last_available_id = buffer_cursor;
    return this->buffer->element_at(this->current_id);
}

} // namespace disruptor
