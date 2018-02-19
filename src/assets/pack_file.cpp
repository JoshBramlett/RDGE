#include <rdge/assets/pack_file.hpp>
#include <rdge/assets/file_formats/asset_pack.hpp>
#include <rdge/assets/spritesheet.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/assets/tileset.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/json.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <SDL_assert.h>

#include <memory>

namespace rdge {

using namespace rdge::asset_pack;
using json = nlohmann::json;

PackFile::PackFile (const char* filepath)
    : m_file(rwops_base::from_file(filepath, "rb"))
{
    if (RDGE_UNLIKELY(m_file.size() < sizeof(header)))
    {
        RDGE_THROW("Invalid file");
    }

    m_file.seek(0, rwops_base::seekdir::beg);
    m_file.read(&m_header, sizeof(header));
    if (RDGE_UNLIKELY(m_header.magic_value != RDGE_MAGIC_VALUE))
    {
        RDGE_THROW("Unrecognized file format");
    }

    if (RDGE_UNLIKELY(m_header.version > RDGE_ASSET_PACK_VERSION))
    {
        RDGE_THROW("Pack file is newer than supported");
    }

    if (RDGE_UNLIKELY(m_header.asset_count == 0))
    {
        RDGE_THROW("Pack file asset table is empty");
    }

    if (RDGE_UNLIKELY(!RDGE_MALLOC_N(m_table, m_header.asset_count, nullptr)))
    {
        RDGE_THROW("Failed to allocate memory");
    }

    if (RDGE_UNLIKELY(!RDGE_CALLOC(m_cache, m_header.asset_count, nullptr)))
    {
        RDGE_THROW("Failed to allocate memory");
    }

    m_file.seek(static_cast<int64>(m_header.assets), rwops_base::seekdir::beg);
    m_file.read(m_table, sizeof(asset_info), m_header.asset_count);
}

PackFile::~PackFile (void) noexcept
{
    RDGE_FREE(m_table, nullptr);

    for (uint32 i = 0; i < m_header.asset_count; i++)
    {
        RDGE_FREE(m_cache[i].asset, nullptr);
    }

    RDGE_FREE(m_cache, nullptr);
}

PackFile::PackFile (PackFile&& other) noexcept
    : m_file(std::move(other.m_file))
    , m_header(other.m_header)
    , m_table(other.m_table)
    , m_cache(other.m_cache)
{ }

PackFile&
PackFile::operator= (PackFile&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_file = std::move(rhs.m_file);
        m_header = rhs.m_header;
        std::swap(m_table, rhs.m_table);
        std::swap(m_cache, rhs.m_cache);
    }

    return *this;
}

template <>
shared_asset<Surface>
PackFile::GetAsset (int32 asset_id)
{
    SDL_assert(asset_id >= 0 && (uint32)asset_id < m_header.asset_count);

    auto& data = m_cache[asset_id];
    if (!data.asset)
    {
        auto& info = m_table[asset_id];
        SDL_assert(info.type == asset_type_surface);

        void* pixel_data = nullptr;
        if (RDGE_UNLIKELY(!RDGE_MALLOC(pixel_data, info.size, nullptr)))
        {
            RDGE_THROW("Memory allocation failed");
        }

        void* asset_memory = nullptr;
        if (RDGE_UNLIKELY(!RDGE_MALLOC(asset_memory, sizeof(Surface), nullptr)))
        {
            RDGE_THROW("Memory allocation failed");
        }

        m_file.seek(info.offset, rwops_base::seekdir::beg);
        m_file.read(pixel_data, info.size);

        data.ref_count = 0;
        data.asset_id = asset_id;
        data.type = info.type;
        data.lifetime = SharedAssetLifetime::REF_COUNT_MANAGED;
        data.asset = new (asset_memory) Surface(pixel_data,
                                                info.surface.width,
                                                info.surface.height,
                                                info.surface.channels);

        ILOG() << "Asset Loaded:"
               << " asset_id=" << data.asset_id
               << " type=" << data.type
               << " size=" << info.size;
    }

    SDL_assert(data.type == asset_type_surface);
    return shared_asset<Surface>(static_cast<Surface*>(data.asset), &data);
}

template <>
shared_asset<SpriteSheet>
PackFile::GetAsset (int32 asset_id)
{
    SDL_assert(asset_id >= 0 && (uint32)asset_id < m_header.asset_count);

    auto& data = m_cache[asset_id];
    if (!data.asset)
    {
        auto& info = m_table[asset_id];
        SDL_assert(info.type == asset_type_spritesheet);

        void* asset_memory = nullptr;
        if (RDGE_UNLIKELY(!RDGE_MALLOC(asset_memory, sizeof(SpriteSheet), nullptr)))
        {
            RDGE_THROW("Memory allocation failed");
        }

        std::vector<std::uint8_t> msgpack(info.size);
        m_file.seek(info.offset, rwops_base::seekdir::beg);
        m_file.read(msgpack.data(), info.size);

        data.ref_count = 0;
        data.asset_id = asset_id;
        data.type = info.type;
        data.lifetime = SharedAssetLifetime::REF_COUNT_MANAGED;
        data.asset = new (asset_memory) SpriteSheet(msgpack, *this);

        ILOG() << "Asset Loaded:"
               << " asset_id=" << data.asset_id
               << " type=" << data.type
               << " size=" << info.size;
    }

    SDL_assert(data.type == asset_type_spritesheet);
    return shared_asset<SpriteSheet>(static_cast<SpriteSheet*>(data.asset), &data);
}

template <>
shared_asset<Tileset>
PackFile::GetAsset (int32 asset_id)
{
    SDL_assert(asset_id >= 0 && (uint32)asset_id < m_header.asset_count);

    auto& data = m_cache[asset_id];
    if (!data.asset)
    {
        auto& info = m_table[asset_id];
        SDL_assert(info.type == asset_type_tileset);

        void* asset_memory = nullptr;
        if (RDGE_UNLIKELY(!RDGE_MALLOC(asset_memory, sizeof(Tileset), nullptr)))
        {
            RDGE_THROW("Memory allocation failed");
        }

        std::vector<std::uint8_t> msgpack(info.size);
        m_file.seek(info.offset, rwops_base::seekdir::beg);
        m_file.read(msgpack.data(), info.size);

        data.ref_count = 0;
        data.asset_id = asset_id;
        data.type = info.type;
        data.lifetime = SharedAssetLifetime::REF_COUNT_MANAGED;
        data.asset = new (asset_memory) Tileset(msgpack, *this);

        ILOG() << "Asset Loaded:"
               << " asset_id=" << data.asset_id
               << " type=" << data.type
               << " size=" << info.size;
    }

    SDL_assert(data.type == asset_type_tileset);
    return shared_asset<Tileset>(static_cast<Tileset*>(data.asset), &data);
}

template <>
shared_asset<tilemap::Tilemap>
PackFile::GetAsset (int32 asset_id)
{
    using namespace rdge::tilemap;
    SDL_assert(asset_id >= 0 && (uint32)asset_id < m_header.asset_count);

    auto& data = m_cache[asset_id];
    if (!data.asset)
    {
        auto& info = m_table[asset_id];
        SDL_assert(info.type == asset_type_tilemap);

        void* asset_memory = nullptr;
        if (RDGE_UNLIKELY(!RDGE_MALLOC(asset_memory, sizeof(Tilemap), nullptr)))
        {
            RDGE_THROW("Memory allocation failed");
        }

        std::vector<std::uint8_t> msgpack(info.size);
        m_file.seek(info.offset, rwops_base::seekdir::beg);
        m_file.read(msgpack.data(), info.size);

        data.ref_count = 0;
        data.asset_id = asset_id;
        data.type = info.type;
        data.lifetime = SharedAssetLifetime::REF_COUNT_MANAGED;
        data.asset = new (asset_memory) Tilemap(msgpack, *this);

        ILOG() << "Asset Loaded:"
               << " asset_id=" << data.asset_id
               << " type=" << data.type
               << " size=" << info.size;
    }

    SDL_assert(data.type == asset_type_tilemap);
    return shared_asset<Tilemap>(static_cast<Tilemap*>(data.asset), &data);
}

} // namespace rdge
