#include <rdge/assets/asset_pack.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_assert.h>

#include <memory>

namespace rdge
{

using namespace rdge::asset_pack;

PackFile::PackFile (const char* filepath)
    : m_file(rwops_base::from_file(filepath, "rb"))
{
    if (UNLIKELY(m_file.size() < sizeof(header)))
    {
        RDGE_THROW("Invalid file");
    }

    m_file.seek(0, rwops_base::seekdir::beg);
    m_file.read(&m_header, sizeof(header));
    if (UNLIKELY(m_header.magic_value != RDGE_MAGIC_VALUE))
    {
        RDGE_THROW("Unrecognized file format");
    }

    if (UNLIKELY(m_header.version > RDGE_ASSET_PACK_VERSION))
    {
        RDGE_THROW("Pack file is newer than supported");
    }

    if (UNLIKELY(m_header.asset_count == 0))
    {
        RDGE_THROW("Pack file asset table is empty");
    }

    RDGE_MALLOC_N(m_table, m_header.asset_count, nullptr);
    m_file.seek(static_cast<int64>(m_header.assets), rwops_base::seekdir::beg);
    m_file.read(m_table, sizeof(asset_info), m_header.asset_count);
}

PackFile::~PackFile (void) noexcept
{
    RDGE_FREE(m_table, nullptr);
}

PackFile::PackFile (PackFile&& other) noexcept
    : m_file(std::move(other.m_file))
    , m_header(other.m_header)
    , m_table(other.m_table)
{ }

PackFile&
PackFile::operator= (PackFile&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_file = std::move(rhs.m_file);
        m_header = rhs.m_header;
        std::swap(m_table, rhs.m_table);
    }

    return *this;
}

Surface
PackFile::GetSurface (int32 asset_id)
{
    SDL_assert(asset_id >= 0 && (uint32)asset_id < m_header.asset_count);

    auto& info = m_table[asset_id];
    SDL_assert(info.type == asset_type_surface);

    void* pixel_data = malloc(info.size);
    // TODO replace when stb_image uses RDGE_MALLOC
    //RDGE_MALLOC(pixel_data, info.size, nullptr);
    m_file.seek(info.offset, rwops_base::seekdir::beg);
    m_file.read(pixel_data, info.size);

    return Surface(pixel_data, info.surface.width, info.surface.height, info.surface.channels);
}

SpriteSheet
PackFile::GetSpriteSheet (int32 asset_id)
{
    SDL_assert(asset_id >= 0 && (uint32)asset_id < m_header.asset_count);

    auto& info = m_table[asset_id];
    std::vector<std::uint8_t> msgpack(info.size);
    m_file.seek(info.offset, rwops_base::seekdir::beg);
    m_file.read(msgpack.data(), info.size);

    if (info.type == asset_type_spritesheet)
    {
        return SpriteSheet(msgpack, GetSurface(info.spritesheet.surface_id));
    }
    else if (info.type == asset_type_tilemap)
    {
        return SpriteSheet(msgpack, GetSurface(info.tilemap.surface_id));
    }
    else
    {
        SDL_assert(false);
    }

    return SpriteSheet();
}

} // namespace rdge
