//! \headerfile <rdge/util/containers/nodeless_forward_list.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/22/2017

#pragma once

#include <rdge/core.hpp>

#include <SDL_assert.h>

#include <functional>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct nodeless_forward_list
//! \brief Forward linked list where the templated type is the node
//! \details Avoids wrapping the type in a node to avoid an uneccessary
//!          dereference.  The templated type must contain a public
//!          member pointer named 'next' in order to be compatible.
//!          This member must NEVER be modified manually.
//! \code{.cpp}
//! struct foo
//! {
//!     foo* next = nullptr;
//! };
//! \endcode
//! \warning Container does not take ownership of the list elements, so
//!          memory management must be done externally.
template <typename T>
struct nodeless_forward_list
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
#ifdef RDGE_DEBUG
        size_t old_count = this->count;
#endif
        SDL_assert(count > 0);
        SDL_assert(element != nullptr);

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

#ifdef RDGE_DEBUG
        SDL_assert(old_count == (count + 1));
#endif
    }

    //! \brief Call the provided function for each member of the list
    //! \param [in] fn Function called for each element
    void for_each (std::function<void(T*)> fn)
    {
        T* cursor = this->first;
        while (cursor)
        {
            fn(cursor);
            cursor = cursor->next;
        }
    }
};

} // namespace rdge
