#include <rdge/assets/pack_file.hpp>
#include <rdge/assets/file_formats/asset_pack.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/util/json.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_assert.h>

#include <memory>

namespace rdge {

using namespace rdge::asset_pack;
using json = nlohmann::json;

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

    void* pixel_data = nullptr;
    RDGE_MALLOC(pixel_data, info.size, nullptr);
    m_file.seek(info.offset, rwops_base::seekdir::beg);
    m_file.read(pixel_data, info.size);

    auto& s_info = info.surface;
    return Surface(pixel_data, s_info.width, s_info.height, s_info.channels);
}

SpriteSheet
PackFile::GetSpriteSheet (int32 asset_id)
{
    SDL_assert(asset_id >= 0 && (uint32)asset_id < m_header.asset_count);

    auto& info = m_table[asset_id];
    SDL_assert(info.type == asset_type_spritesheet);

    std::vector<std::uint8_t> msgpack(info.size);
    m_file.seek(info.offset, rwops_base::seekdir::beg);
    m_file.read(msgpack.data(), info.size);

    SpriteSheet result(msgpack, *this);
    return result;
    //return SpriteSheet(msgpack, *this);
}

Tileset
PackFile::GetTileset (int32 asset_id)
{
    SDL_assert(asset_id >= 0 && (uint32)asset_id < m_header.asset_count);

    auto& info = m_table[asset_id];
    SDL_assert(info.type == asset_type_tileset);

    std::vector<std::uint8_t> msgpack(info.size);
    m_file.seek(info.offset, rwops_base::seekdir::beg);
    m_file.read(msgpack.data(), info.size);

    Tileset result(msgpack, *this);
    return result;
    //return Tileset(msgpack, *this);
}

tilemap::Tilemap
PackFile::GetTilemap (int32 asset_id)
{
    SDL_assert(asset_id >= 0 && (uint32)asset_id < m_header.asset_count);

    auto& info = m_table[asset_id];
    SDL_assert(info.type == asset_type_tilemap);

    std::vector<std::uint8_t> msgpack(info.size);
    m_file.seek(info.offset, rwops_base::seekdir::beg);
    m_file.read(msgpack.data(), info.size);

    return tilemap::Tilemap(json::from_msgpack(msgpack));
}

} // namespace rdge
