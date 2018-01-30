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

//! \class intrusive_forward_list_iterator
//! \brief Unidirectional iterator for an intrusive list
template <typename T, typename Pointer = T*, typename Reference = T&>
class intrusive_forward_list_iterator
{
private:
    // brevity
    using this_type = intrusive_forward_list_iterator<T, Pointer, Reference>;
public:

    //!@{ iterator traits
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = Pointer;
    using reference = Reference;
    //!@}

    explicit intrusive_forward_list_iterator (pointer ptr = nullptr) : m_ptr(ptr) { }
    ~intrusive_forward_list_iterator (void) noexcept = default;

    intrusive_forward_list_iterator (const this_type&) = default;
    this_type& operator= (const this_type&) = default;

    //!@{ member access
    reference operator* (void) const { return *m_ptr; }
    pointer operator-> (void) const { return m_ptr; }
    //!@}

    //!@{ pre/post increment
    this_type& operator++ (void) { m_ptr = m_ptr->next; return *this; }

    intrusive_forward_list_iterator operator++ (int)
    {
        intrusive_forward_list_iterator it(m_ptr);
        m_ptr = m_ptr->next;
        return it;
    }
    //!@}

    //!@{ comparison
    friend bool operator== (const this_type& lhs, const this_type& rhs)
        { return lhs.m_ptr == rhs.m_ptr; }
    friend bool operator!= (const this_type& lhs, const this_type& rhs)
        { return lhs.m_ptr != rhs.m_ptr; }
    //!@}

protected:
    pointer m_ptr;
};

//! \class intrusive_list_iterator
//! \brief Bidirectional iterator for an intrusive list
template <typename T, typename Pointer = T*, typename Reference = T&>
class intrusive_list_iterator
{
private:
    // brevity
    using this_type = intrusive_list_iterator<T, Pointer, Reference>;
public:

    //!@{ iterator traits
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = Pointer;
    using reference = Reference;
    //!@}

    explicit intrusive_list_iterator (pointer ptr = nullptr) : m_ptr(ptr) { }
    ~intrusive_list_iterator (void) noexcept = default;

    intrusive_list_iterator (const this_type&) = default;
    this_type& operator= (const this_type&) = default;

    //!@{ member access
    reference operator* (void) const { return *m_ptr; }
    pointer operator-> (void) const { return m_ptr; }
    //!@}

    //!@{ pre/post increment/decrement
    this_type& operator++ (void) { m_ptr = m_ptr->next; return *this; }
    this_type& operator-- (void) { m_ptr = m_ptr->prev; return *this; }

    intrusive_list_iterator operator++ (int)
    {
        intrusive_list_iterator it(m_ptr);
        m_ptr = m_ptr->next;
        return it;
    }

    intrusive_list_iterator operator-- (int)
    {
        intrusive_list_iterator it(m_ptr);
        m_ptr = m_ptr->prev;
        return it;
    }
    //!@}

    //!@{ comparison
    friend bool operator== (const this_type& lhs, const this_type& rhs)
        { return lhs.m_ptr == rhs.m_ptr; }
    friend bool operator!= (const this_type& lhs, const this_type& rhs)
        { return lhs.m_ptr != rhs.m_ptr; }
    //!@}

private:
    pointer m_ptr;
};

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

    //!@{ member access
    reference operator* (void) const { return *m_ptr; }
    pointer operator-> (void) const { return m_ptr; }
    reference operator[] (difference_type d) const { return m_ptr[d]; }
    //!@}

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
