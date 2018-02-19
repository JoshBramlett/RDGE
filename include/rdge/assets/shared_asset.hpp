//! \headerfile <rdge/assets/shared_asset.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 01/20/2018

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/file_formats/asset_pack.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <SDL_assert.h>

#include <type_traits>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \enum SharedAssetLifetime
//! \brief Behavior for when to destroy the shared asset
enum class SharedAssetLifetime
{
    REF_COUNT = 0,     //!< Typical std::shared_ptr behavior
    REF_COUNT_MANAGED, //!< Reference control block externally managed
    PERSIST            //!< Persist in cache unless explicitly destroyed
};

namespace detail {

//! \struct shared_asset_data
//! \brief Internal shared asset container
struct shared_asset_data
{
    size_t ref_count;
    void* asset;
    int32 asset_id;
    asset_pack::asset_type type;
    SharedAssetLifetime lifetime;
};

} // namespace detail

//! \class shared_asset
//! \brief User friendly \ref shared_asset_data wrapper
//! \details Has similar functionality to a std::shared_ptr with reference counting
//!          and automatic deletion, but extends to the specific use of managing
//!          shared assets.  By default the wrapper behaves the same as a
//!          std::shared_ptr, but can be overridden to only manage the asset and
//!          leave the detail data unmanaged, or to not manage the asset whatsoever.
template <typename T>
class shared_asset
{
public:
    //!@{ pointer traits
    using element_type = T;
    using pointer = T*;
    //!@}

    //! \brief shared_asset default ctor
    constexpr shared_asset (void) noexcept = default;

    //! \brief shared_asset nullptr ctor
    constexpr shared_asset (std::nullptr_t) noexcept
        : m_ptr(nullptr)
        , m_block(nullptr)
    { }

    //! \brief shared_asset ctor
    //! \details Construct shared_asset from a pre-allocated pointer
    //! \param [in] ptr Pre-allocated pointer
    explicit shared_asset (pointer ptr)
        : m_ptr(ptr)
    {
        if (RDGE_UNLIKELY(!RDGE_CALLOC(m_block, 1, nullptr)))
        {
            RDGE_THROW("Failed to allocate memory");
        }

        m_block->ref_count = 1;
        m_block->asset = m_ptr;
        m_block->asset_id = -1;
        m_block->type = asset_pack::asset_type_invalid;
        m_block->lifetime = SharedAssetLifetime::REF_COUNT;
    }

    //! \brief shared_asset ctor
    //! \details Construct shared_asset from internal data
    //! \param [in] ptr Pre-allocated pointer
    //! \param [in] block Externally managed data
    explicit shared_asset (pointer ptr, detail::shared_asset_data* block)
        : m_ptr(ptr)
        , m_block(block)
    {
        m_block->ref_count++;
    }

    //! \brief shared_asset dtor
    ~shared_asset (void) noexcept
    {
        dereference();
    }

    //!@{ Copy and move enabled
    shared_asset (const shared_asset& other)
        : m_ptr(other.m_ptr)
        , m_block(other.m_block)
    {
        if (m_block)
        {
            m_block->ref_count++;
        }
    }

    shared_asset& operator= (const shared_asset& rhs)
    {
        if (m_block != rhs.m_block)
        {
            dereference();

            m_ptr = rhs.m_ptr;
            m_block = rhs.m_block;
            if (m_block)
            {
                m_block->ref_count++;
            }
        }

        return *this;
    }

    shared_asset (shared_asset&& other) noexcept
        : m_ptr(other.m_ptr)
        , m_block(other.m_block)
    {
        other.m_ptr = nullptr;
        other.m_block = nullptr;
    }

    shared_asset& operator= (shared_asset&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(m_ptr, rhs.m_ptr);
            std::swap(m_block, rhs.m_block);
        }

        return *this;
    }
    //!@}

    //!@{ Comparison operators
    explicit operator bool (void) const noexcept { return m_ptr != nullptr; }
    bool operator== (std::nullptr_t) const noexcept { return m_ptr == nullptr; }
    bool operator!= (std::nullptr_t) const noexcept { return m_ptr != nullptr; }
    //!@}

    //!@{ Member access operators
    typename std::add_lvalue_reference<T>::type operator* (void) const { return *m_ptr; }
    pointer operator-> (void) const noexcept { return m_ptr; }
    //!@}

    //!@{ Wrapper/Asset accessors
    pointer get (void) const noexcept { return m_ptr; }
    bool empty (void) const noexcept { return m_ptr == nullptr; }
    size_t use_count (void) const noexcept
        { return (!m_block ? 0 : m_block->ref_count); }
    int32 asset_id (void) const noexcept
        { return (!m_block ? -1 : m_block->asset_id); }
    asset_pack::asset_type asset_type (void) const noexcept
        { return (!m_block ? asset_pack::asset_type_invalid : m_block->type); }
    //!@}

private:

    void dereference (void)
    {
        if (m_block)
        {
            SDL_assert(m_block->ref_count > 0);
            m_block->ref_count--;
            if (m_block->ref_count == 0)
            {
                if (m_block->lifetime == SharedAssetLifetime::REF_COUNT)
                {
                    m_ptr->~T();
                    RDGE_FREE(m_ptr, nullptr);
                    RDGE_FREE(m_block, nullptr);
                }
                else if (m_block->lifetime == SharedAssetLifetime::REF_COUNT_MANAGED)
                {
                    m_ptr->~T();
                    RDGE_FREE(m_ptr, nullptr);
                    m_block->asset = nullptr;
                }
            }
        }
    }

    pointer m_ptr = nullptr;
    detail::shared_asset_data* m_block = nullptr;
};

} // namespace rdge
