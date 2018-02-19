//! \headerfile <rdge/assets/pack_file.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 09/20/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/file_formats/asset_pack.hpp>
#include <rdge/assets/shared_asset.hpp>
#include <rdge/assets/tilemap/tilemap.hpp>
#include <rdge/util/io/rwops_base.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

class PackFile
{
public:
    //! \brief PackFile ctor
    //! \details Open an asset pack file for reading
    //! \param [in] filepath Path to pack file
    //! \throws rdge::Exception Invalid file
    //! \throws rdge::SDLException File cannot be loaded
    explicit PackFile (const char* filepath);

    //! \brief PackFile dtor
    ~PackFile (void) noexcept;

    //!@{ Non-copyable, move enabled
    PackFile (const PackFile&) = delete;
    PackFile& operator= (const PackFile&) = delete;
    PackFile (PackFile&&) noexcept;
    PackFile& operator= (PackFile&&) noexcept;
    //!@}

    //void PreloadAsset (int32 asset_id, SharedAssetLifetime lifetime);

    //! \brief Get a constructed asset from the asset packed file
    //! \details Assets will be cached and not reconstructed every call
    //! \param [in] asset_id Index of the asset in the table
    //! \throws rdge::Exception Unable to generate asset
    template <typename T>
    shared_asset<T> GetAsset (int32 asset_id);

private:
    rwops_base m_file;

    asset_pack::header         m_header;
    asset_pack::asset_info*    m_table = nullptr;
    detail::shared_asset_data* m_cache = nullptr;
};

} // namespace rdge
