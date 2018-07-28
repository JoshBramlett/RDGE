//! \headerfile <rdge/util/containers/intrusive_list.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/22/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/containers/iterators.hpp>
#include <rdge/util/logger.hpp>

#include <SDL_assert.h>

#include <functional>
#include <iterator>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct intrusive_list_element
//! \brief CRTP base class for the list elements
//! \details Inheriting from this class contains the 'next' pointer
//!          required by the \ref intrusive_list.
template <typename T>
struct intrusive_list_element
{
    virtual ~intrusive_list_element (void) noexcept = default;

    T& operator* (void) { return *static_cast<T*>(this); }
    const T& operator* (void) const { return *static_cast<const T*>(this); }
    T* operator& (void) { return static_cast<T*>(this); }
    const T* operator& (void) const { return static_cast<const T*>(this); }

    T* prev = nullptr; //!< Pointer to the previous element
    T* next = nullptr; //!< Pointer to the next element
};

//! \struct intrusive_list
//! \brief Doubly linked list where the templated type is the node
//! \details Avoids wrapping the type in a node to avoid an uneccessary
//!          dereference.  The templated type must contain public
//!          member pointers 'prev' and 'next' in order to be compatible.
//!          Alternatively the templated type may inherit the \ref
//!          intrusive_list_element that contains the pointers.
//!          These members must NEVER be modified manually.
//! \code{.cpp}
//! struct foo
//! {
//!     foo* prev = nullptr;
//!     foo* next = nullptr;
//!     int32 some_value;
//! };
//!
//! struct bar : public intrusive_list_element<bar>
//! {
//!     int32 some_value;
//! };
//! \endcode
//! \warning Container does not take ownership of the list elements, so
//!          memory management must be done externally.
template <typename T>
class intrusive_list
{
public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    using iterator = detail::intrusive_list_iterator<value_type>;
    using const_iterator = detail::intrusive_list_iterator<T, const T*, const T&>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    //!@{ intrusive_list default ctor/dtor
    intrusive_list (void) noexcept
    {
        m_anchor.prev = &m_anchor;
        m_anchor.next = &m_anchor;
    }

    ~intrusive_list (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    intrusive_list (const intrusive_list&) = delete;
    intrusive_list& operator= (const intrusive_list&) = delete;

    intrusive_list (intrusive_list&& other) noexcept
        : m_count(other.m_count)
    {
        m_anchor.prev = &m_anchor;
        m_anchor.next = &m_anchor;

        if (other.m_anchor.prev != &other.m_anchor)
        {
            m_anchor.prev = other.m_anchor.prev;
            m_anchor.prev->next = &m_anchor;
            other.m_anchor.prev = &other.m_anchor;
        }

        if (other.m_anchor.next != &other.m_anchor)
        {
            m_anchor.next = other.m_anchor.next;
            m_anchor.next->prev = &m_anchor;
            other.m_anchor.next = &other.m_anchor;
        }

        other.m_count = 0;
    }

    intrusive_list& operator= (intrusive_list&& rhs) noexcept
    {
        if (this != &rhs)
        {
            m_count = rhs.m_count;
            rhs.m_count = 0;

            if (rhs.m_anchor.prev != &rhs.m_anchor)
            {
                m_anchor.prev = rhs.m_anchor.prev;
                m_anchor.prev->next = &m_anchor;
                rhs.m_anchor.prev = &rhs.m_anchor;
            }

            if (rhs.m_anchor.next != &rhs.m_anchor)
            {
                m_anchor.next = rhs.m_anchor.next;
                m_anchor.next->prev = &m_anchor;
                rhs.m_anchor.next = &rhs.m_anchor;
            }
        }

        return *this;
    }
    //!@}

    //!@{ Forward iterator
    RDGE_ALWAYS_INLINE iterator begin (void) noexcept
    {
        return iterator(m_anchor.next);
    }

    RDGE_ALWAYS_INLINE const_iterator begin (void) const noexcept
    {
        return const_iterator(m_anchor.next);
    }

    RDGE_ALWAYS_INLINE const_iterator cbegin (void) const noexcept
    {
        return const_iterator(m_anchor.next);
    }

    RDGE_ALWAYS_INLINE iterator end (void) noexcept
    {
        return iterator(&m_anchor);
    }

    RDGE_ALWAYS_INLINE const_iterator end (void) const noexcept
    {
        return const_iterator(&m_anchor);
    }

    RDGE_ALWAYS_INLINE const_iterator cend (void) const noexcept
    {
        return const_iterator(&m_anchor);
    }
    //!@}

    //!@{ Reverse iterator
    reverse_iterator rbegin (void) noexcept
    {
        return reverse_iterator(iterator(&m_anchor));
    }

    const_reverse_iterator rbegin (void) const noexcept
    {
        return const_reverse_iterator(const_iterator(&m_anchor));
    }

    const_reverse_iterator crbegin (void) const noexcept
    {
        return const_reverse_iterator(const_iterator(&m_anchor));
    }

    reverse_iterator rend (void) noexcept
    {
        return reverse_iterator(iterator(m_anchor.next));
    }

    const_reverse_iterator rend (void) const noexcept
    {
        return const_reverse_iterator(const_iterator(m_anchor.next));
    }

    const_reverse_iterator crend (void) const noexcept
    {
        return const_reverse_iterator(const_iterator(m_anchor.next));
    }
    //!@}

    //!@{ Container properties
    RDGE_ALWAYS_INLINE size_t size (void) const noexcept { return m_count; }
    RDGE_ALWAYS_INLINE bool empty (void) const noexcept { return (m_count == 0); }
    //!@}

    //!@{ Element access
    reference front (void) { SDL_assert(!empty()); return *m_anchor.next; }
    const_reference front (void) const { SDL_assert(!empty()); return *m_anchor.next; }
    reference back (void) { SDL_assert(!empty()); return *m_anchor.prev; }
    const_reference back (void) const { SDL_assert(!empty()); return *m_anchor.prev; }
    //!@}

    //! \brief Add an element to the start of the list
    //! \note Insertion complexity: O(1)
    //! \param [in] element Element to be added
    void push_front (reference element)
    {
        // Unclean or in another collection
        SDL_assert(element.next == nullptr);
        SDL_assert(element.prev == nullptr);

        element.prev = &m_anchor;
        element.next = m_anchor.next;
        m_anchor.next = &element;
        element.next->prev = &element;
        m_count++;
    }

    //! \brief Add an element to the end of the list
    //! \note Insertion complexity: O(1)
    //! \param [in] element Element to be added
    void push_back (reference element)
    {
        // Unclean or in another collection
        SDL_assert(element.next == nullptr);
        SDL_assert(element.prev == nullptr);

        element.prev = m_anchor.prev;
        element.next = &m_anchor;
        m_anchor.prev = &element;
        element.prev->next = &element;
        m_count++;
    }

    //! \brief Insert an element before the iterator position
    //! \note Insertion complexity: O(1)
    //! \param [in] pos Iterator to the insert position
    //! \param [in] element Element to be added
    //! \returns Iterator to the inserted element
    iterator insert (iterator pos, reference element)
    {
        // Unclean or in another collection
        SDL_assert(element.next == nullptr);
        SDL_assert(element.prev == nullptr);

        element.prev = pos->prev;
        element.next = &(*pos);
        pos->prev = &element;
        element.prev->next = &element;
        m_count++;

        return iterator(&element);
    }

    //! \brief Remove an element from the list
    //! \param [in] element Element to be removed
    void remove (reference element)
    {
        SDL_assert(m_count > 0);
        SDL_assert(contains(element));
        SDL_assert(element.next != nullptr);
        SDL_assert(element.prev != nullptr);

        element.prev->next = element.next;
        element.next->prev = element.prev;

        element.prev = nullptr;
        element.next = nullptr;
        m_count--;
    }

    //! \brief Clear the list (invalidating all pointers)
    void clear (void) noexcept
    {
        pointer cursor = m_anchor.next;
        while (cursor != &m_anchor)
        {
            pointer next = cursor->next;
            cursor->prev = nullptr;
            cursor->next = nullptr;
            cursor = next;
        }

        m_anchor.prev = &m_anchor;
        m_anchor.next = &m_anchor;
        m_count = 0;
    }

    //! \brief Check if an element is contained in the list
    //! \param [in] element Element to be checked
    //! \returns True iff the list contains the element
    bool contains (reference element) const noexcept
    {
        for (const_pointer p = m_anchor.next; p != &m_anchor; p = p->next)
        {
            if (p == &element)
            {
                return true;
            }
        }

        return false;
    }

    //! \brief Sort the items in the list using the provided comparator
    //! \details Algorithm based on bubble sort.  Be cognizant of use, bubble
    //!          sort is suboptimal unless list is small or nearly sorted.
    //! \note Sort complexity: O(n^2)
    //! \param [in] fn Comparison function
    void sort (std::function<bool(const_reference, const_reference)> fn)
    {
        bool sorted = false;
        while (!sorted)
        {
            sorted = true;
            pointer cursor = m_anchor.next;
            while (cursor != &m_anchor)
            {
                pointer ncursor = cursor->next;
                if (ncursor != &m_anchor && !fn(*cursor, *ncursor))
                {
                    cursor->prev->next = ncursor;
                    ncursor->next->prev = cursor;

                    ncursor->prev = cursor->prev;
                    cursor->prev = ncursor;
                    cursor->next = ncursor->next;
                    ncursor->next = cursor;

                    sorted = false;
                }
                else
                {
                    cursor = ncursor;
                }
            }
        }
    }

    //! \brief Call the provided function for each member of the list
    //! \param [in] fn Function called for each element
    void for_each (std::function<void(pointer)> fn)
    {
        pointer cursor = m_anchor.next;
        while (cursor != &m_anchor)
        {
            // store next element in case callee modifies it's element pointers
            // (e.g. removes iteself from the list)
            pointer cached_cursor = cursor->next;
            fn(cursor);
            cursor = cached_cursor;
        }
    }

private:
    intrusive_list_element<value_type> m_anchor;
    size_t m_count = 0;
};

//! \struct intrusive_forward_list_element
//! \brief CRTP base class for the list elements
//! \details Inheriting from this class contains the 'next' pointer
//!          required by the \ref intrusive_forward_list.
template <typename T>
struct intrusive_forward_list_element
{
    virtual ~intrusive_forward_list_element (void) noexcept = default;

    T* next = nullptr; //!< Pointer to the next element
};

//! \struct intrusive_forward_list
//! \brief Forward linked list where the templated type is the node
//! \details Avoids wrapping the type in a node to avoid an uneccessary
//!          dereference.  The templated type must contain a public
//!          member pointer named 'next' in order to be compatible.
//!          Alternatively the templated type may inherit the \ref
//!          intrusive_forward_list_element that contains the pointer.
//!          This member must NEVER be modified manually.
//! \code{.cpp}
//! struct foo
//! {
//!     foo* next = nullptr;
//!     int32 some_value;
//! };
//!
//! struct bar : public intrusive_forward_list_element<bar>
//! {
//!     int32 some_value;
//! };
//! \endcode
//! \warning Container does not take ownership of the list elements, so
//!          memory management must be done externally.
template <typename T>
class intrusive_forward_list
{
public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    using iterator = detail::intrusive_forward_list_iterator<T>;
    using const_iterator = detail::intrusive_forward_list_iterator<T, const T*, const T&>;

    //!@{ intrusive_list default ctor/dtor
    intrusive_forward_list (void) noexcept = default;
    ~intrusive_forward_list (void) noexcept = default;
    //!@}

    //!@{ Non-copyable, move enabled
    intrusive_forward_list (const intrusive_forward_list&) = delete;
    intrusive_forward_list& operator= (const intrusive_forward_list&) = delete;

    intrusive_forward_list (intrusive_forward_list&& other) noexcept
        : m_first(other.m_first)
        , m_count(other.m_count)
    {
        other.m_first = nullptr;
        other.m_count = 0;
    }

    intrusive_forward_list& operator= (intrusive_forward_list&& rhs) noexcept
    {
        if (this != &rhs)
        {
            m_first = rhs.m_first;
            rhs.m_first = nullptr;

            m_count = rhs.m_count;
            rhs.m_count = 0;
        }

        return *this;
    }
    //!@}

    //!@{ Forward iterator
    RDGE_ALWAYS_INLINE iterator begin (void) { return iterator(m_first); }
    RDGE_ALWAYS_INLINE const_iterator begin (void) const { return const_iterator(m_first); }
    RDGE_ALWAYS_INLINE const_iterator cbegin (void) const { return const_iterator(m_first); }
    RDGE_ALWAYS_INLINE iterator end (void) { return iterator(); }
    RDGE_ALWAYS_INLINE const_iterator end (void) const { return const_iterator(); }
    RDGE_ALWAYS_INLINE const_iterator cend (void) const { return const_iterator(); }
    //!@}

    //!@{ Container properties
    RDGE_ALWAYS_INLINE size_t size (void) const noexcept { return m_count; }
    RDGE_ALWAYS_INLINE bool empty (void) const noexcept { return (m_count == 0); }
    //!@}

    //!@{ Element access
    reference front (void) { SDL_assert(!empty()); return *m_first; }
    const_reference front (void) const { SDL_assert(!empty()); return *m_first; }
    //!@}

    //! \brief Add an element to the start of the list
    //! \note Complexity: O(1)
    //! \param [in] element Element to be added
    void push_front (reference element)
    {
        SDL_assert(element.next == nullptr); // Unclean or in another collection

        element.next = m_first;
        m_first = &element;
        m_count++;
    }

    //! \brief Add an element to the end of the list
    //! \details Depending on memory layout placing an element on the back of the
    //!          the list may provide better cache locality during iteration.
    //! \note Complexity: O(n)
    //! \param [in] element Element to be added
    void push_back (reference element)
    {
        SDL_assert(element.next == nullptr); // Unclean or in another collection

        T** cursor = &m_first;
        while (*cursor)
        {
            cursor = &(*cursor)->next;
        }

        *cursor = &element;
        m_count++;
    }

    //! \brief Insert an element before the iterator position
    //! \note Insertion complexity: O(1)
    //! \param [in] pos Iterator to the insert position
    //! \param [in] element Element to be added
    //! \returns Iterator to the inserted element
    iterator insert (iterator pos, reference element)
    {
        SDL_assert(element.next == nullptr); // Unclean or in another collection

        T** cursor = &m_first;
        do
        {
            if (*cursor == &(*pos))
            {
                *cursor = &element;
                element.next = &(*pos);

                m_count++;
                return iterator(&element);
            }

            cursor = &(*cursor)->next;
        } while (*cursor);

        SDL_assert(false);
        return iterator(nullptr);
    }

    //! \brief Remove an element from the list
    //! \note Complexity: O(n) (worst-case)
    //! \param [in] element Element to be removed
    void remove (reference element)
    {
        SDL_assert(m_count > 0);

        T** cursor = &m_first;
        while (*cursor)
        {
            if (*cursor == &element)
            {
                *cursor = element.next;
                element.next = nullptr;

                m_count--;
                return;
            }

            cursor = &(*cursor)->next;
        }

        SDL_assert(false); // not in collection
    }

    //! \brief Clear the list (invalidating all pointers)
    void clear (void) noexcept
    {
        pointer cursor = m_first;
        while (cursor)
        {
            auto next = cursor->next;
            cursor->next = nullptr;
            cursor = next;
        }

        m_first = nullptr;
        m_count = 0;
    }

    //! \brief Check if an element is contained in the list
    //! \param [in] element Element to be checked
    //! \returns True iff the list contains the element
    bool contains (const_reference element) const noexcept
    {
        pointer cursor = m_first;
        while (cursor)
        {
            if (cursor == &element)
            {
                return true;
            }

            cursor = cursor->next;
        }

        return false;
    }

    //! \brief Call the provided function for each member of the list
    //! \param [in] fn Function called for each element
    void for_each (std::function<void(pointer)> fn)
    {
        pointer cursor = m_first;
        while (cursor)
        {
            // store next element in case callee modifies it's element pointers
            // (e.g. removes iteself from the list)
            pointer cached_cursor = cursor->next;
            fn(cursor);
            cursor = cached_cursor;
        }
    }

private:
    pointer m_first = nullptr;
    size_t m_count = 0;
};

} // namespace rdge
