//! \headerfile <rdge/util/containers/intrusive_list.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/22/2017

#pragma once

#include <rdge/core.hpp>

#include <SDL_assert.h>

#include <functional>

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
struct intrusive_list
{
    T* first = nullptr;
    T* last = nullptr;
    size_t count = 0;

    //! \brief Add an element to the start of the list
    //! \note Insertion complexity: O(1)
    //! \param [in] element Element to be added
    void push_front (T* element)
    {
        SDL_assert(element != nullptr);

        element->prev = nullptr;
        element->next = this->first;
        if (this->first)
        {
            this->first->prev = element;
        }
        else
        {
            this->last = element;
        }

        this->first = element;
        this->count++;
    }

    //! \brief Add an element to the end of the list
    //! \note Insertion complexity: O(1)
    //! \param [in] element Element to be added
    void push_back (T* element)
    {
        SDL_assert(element != nullptr);

        element->prev = this->last;
        element->next = nullptr;
        if (this->last)
        {
            this->last->next = element;
        }
        else
        {
            this->first = element;
        }

        this->last = element;
        this->count++;
    }

    //! \brief Remove an element from the list
    //! \param [in] element Element to be removed
    void remove (T* element)
    {
        SDL_assert(count > 0);
        SDL_assert(element != nullptr);
        SDL_assert(this->contains(element));

        if (element->prev)
        {
            element->prev->next = element->next;
        }

        if (element->next)
        {
            element->next->prev = element->prev;
        }

        if (element == this->first)
        {
            this->first = element->next;
        }

        if (element == this->last)
        {
            this->last = element->prev;
        }

        element->prev = nullptr;
        element->next = nullptr;
        this->count--;
    }

    //! \brief Check if an element is contained in the list
    //! \param [in] element Element to be checked
    //! \returns True iff the list contains the element
    bool contains (T* element) const noexcept
    {
        T* cursor = this->first;
        while (cursor)
        {
            if (cursor == element)
            {
                return true;
            }

            cursor = cursor->next;
        }

        return false;
    }

    //! \brief Call the provided function for each member of the list
    //! \param [in] fn Function called for each element
    void for_each (std::function<void(T*)> fn)
    {
        T* cursor = this->first;
        while (cursor)
        {
            // store next element in case callee modifies it's element pointers
            // (e.g. removes iteself from the list)
            T* cached_cursor = cursor->next;
            fn(cursor);
            cursor = cached_cursor;
        }
    }

    //! \returns Number of contained elements
    size_t size (void) const noexcept { return this->count; }
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
struct intrusive_forward_list
{
    T* first = nullptr;
    size_t count = 0;

    //! \brief Add an element to the start of the list
    //! \note Insertion complexity: O(1)
    //! \param [in] element Element to be added
    void push_front (T* element)
    {
        SDL_assert(element != nullptr);

        element->next = this->first;
        this->first = element;
        this->count++;
    }

    //! \brief Add an element to the end of the list
    //! \details Depending on memory layout placing an element on the back of the
    //!          the list may provide better cache locality during iteration.
    //! \note Insertion complexity: O(n)
    //! \param [in] element Element to be added
    void push_back (T* element)
    {
        SDL_assert(element != nullptr);

        element->next = nullptr;
        T** cursor = &this->first;
        while (*cursor != nullptr)
        {
            cursor = &(*cursor)->next;
        }

        *cursor = element;
        this->count++;
    }

    //! \brief Remove an element from the list
    //! \param [in] element Element to be removed
    void remove (T* element)
    {
        SDL_assert(count > 0);
        SDL_assert(element != nullptr);
        SDL_assert(this->contains(element));

        T** cursor = &this->first;
        while (*cursor != nullptr)
        {
            if (*cursor == element)
            {
                *cursor = element->next;
                element->next = nullptr;
                this->count--;
                break;
            }

            cursor = &(*cursor)->next;
        }
    }

    //! \brief Check if an element is contained in the list
    //! \param [in] element Element to be checked
    //! \returns True iff the list contains the element
    bool contains (T* element) const noexcept
    {
        T* cursor = this->first;
        while (cursor)
        {
            if (cursor == element)
            {
                return true;
            }

            cursor = cursor->next;
        }

        return false;
    }

    //! \brief Call the provided function for each member of the list
    //! \param [in] fn Function called for each element
    void for_each (std::function<void(T*)> fn)
    {
        T* cursor = this->first;
        while (cursor)
        {
            // store next element in case callee modifies it's element pointers
            // (e.g. removes iteself from the list)
            T* cached_cursor = cursor->next;
            fn(cursor);
            cursor = cached_cursor;
        }
    }

    //! \returns Number of contained elements
    size_t size (void) const noexcept { return this->count; }
};

} // namespace rdge
