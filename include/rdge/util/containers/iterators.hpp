//! \headerfile <rdge/util/containers/iterators.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 06/28/2017

#pragma once

#include <rdge/core.hpp>

#include <iterator>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace detail {

//! \class ra_iterator
//! \brief Random access iterator
template <typename T>
class ra_iterator
{
public:
    //!@{ iterator traits
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    //!@}

    explicit ra_iterator (pointer ptr = nullptr) : m_ptr(ptr) { }
    ~ra_iterator (void) noexcept = default;

    ra_iterator (const ra_iterator&) = default;
    ra_iterator& operator= (const ra_iterator&) = default;

    reference operator* (void) const { return *m_ptr; }
    pointer operator-> (void) const { return m_ptr; }
    reference operator[] (difference_type d) const { return m_ptr[d]; }

    //!@{ pre/post increment/decrement
    ra_iterator& operator++ (void) { ++m_ptr; return *this; }
    ra_iterator& operator-- (void) { --m_ptr; return *this; }
    ra_iterator operator++ (int) { return ra_iterator(m_ptr++); }
    ra_iterator operator-- (int) { return ra_iterator(m_ptr--); }
    //!@}

    //!@{ arithmetic
    ra_iterator& operator+= (difference_type d) { m_ptr += d; return *this; }
    ra_iterator operator+ (difference_type d) const { return ra_iterator(m_ptr + d); }
    friend ra_iterator operator+ (difference_type d, const ra_iterator& rhs)
        { return ra_iterator(d + rhs.m_ptr); }

    ra_iterator& operator-= (difference_type d) { m_ptr -= d; return *this; }
    ra_iterator operator- (difference_type d) const { return ra_iterator(m_ptr - d); }
    difference_type operator- (const ra_iterator& other) { return m_ptr - other.m_ptr; }
    //!@}

    //!@{ comparison
    friend bool operator== (const ra_iterator& lhs, const ra_iterator& rhs)
        { return lhs.m_ptr == rhs.m_ptr; }
    friend bool operator!= (const ra_iterator& lhs, const ra_iterator& rhs)
        { return lhs.m_ptr != rhs.m_ptr; }
    friend bool operator< (const ra_iterator& lhs, const ra_iterator& rhs)
        { return lhs.m_ptr < rhs.m_ptr; }
    friend bool operator> (const ra_iterator& lhs, const ra_iterator& rhs)
        { return lhs.m_ptr > rhs.m_ptr; }
    friend bool operator<= (const ra_iterator& lhs, const ra_iterator& rhs)
        { return lhs.m_ptr <= rhs.m_ptr; }
    friend bool operator>= (const ra_iterator& lhs, const ra_iterator& rhs)
        { return lhs.m_ptr >= rhs.m_ptr; }
    //!@}

private:
    pointer m_ptr;
};

} // namespace detail
} // namespace rdge
