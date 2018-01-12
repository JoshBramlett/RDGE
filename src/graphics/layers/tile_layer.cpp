#include <rdge/graphics/layers/tile_layer.hpp>
#include <rdge/assets/tileset.hpp>
#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/renderers/tile_batch.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_assert.h>

#include <sstream>

namespace rdge {

namespace {

constexpr uint32 FLIPPED_HORIZONTALLY   = 0x80000000;
constexpr uint32 FLIPPED_VERTICALLY     = 0x40000000;
constexpr uint32 FLIPPED_ANTIDIAGONALLY = 0x20000000;

} // anonymous namespace

TileLayer::TileLayer (const tilemap::Layer& def, const Tileset& tileset, float scale)
    : m_offset(def.offset * scale)
    , texture(std::make_shared<Texture>(tileset.surface))
{
    m_cells.count = def.rows * def.cols;
    m_cells.pitch = def.cols;
    m_cells.size = tileset.tile_size * scale;

    // Two provided values make up the total offset.
    //   1) Layer offset (in pixels)
    //   2) start x/y - the tile coordinates of the top/left corner of the layer
    //
    // Coalesce the two, and convert them to y-is-up
    m_offset.y *= -1.f;
    m_offset.x += (m_cells.size.x * static_cast<float>(def.start_x));
    m_offset.y -= (m_cells.size.y * static_cast<float>(def.start_y));

    math::vec2 lo = m_offset;
    math::vec2 hi = m_offset;
    lo.y -= (m_cells.size.y * static_cast<float>(def.rows));
    hi.x += (m_cells.size.x * static_cast<float>(def.cols));
    m_bounds = physics::aabb(lo, hi);

    m_color.a = 255.f * def.opacity;

    if (def.chunks.size() == 1)
    {
        // For fixed size maps:
        //   - Chunk row and column count may differ
        m_chunks.size = math::svec2(def.rows, def.cols);
        m_chunks.pitch = 1;
        m_chunks.count = 1;
    }
    else
    {
        // For dynamically sized maps:
        //   - Chunk row and column count are the same for every chunk
        //   - If there is no data within a chunk it'll be omitted from the definition,
        //     which means we cannot trust the number of chunks as the true count.
        //     For example, say the chunk size is 16x16, and the layer size is 32x32.
        //     If one of the chunks contains no gid references, only three of the
        //     four chunks will be provided.
        m_chunks.size = math::svec2(def.chunks[0].rows, def.chunks[0].cols);
        m_chunks.pitch = def.cols / m_chunks.size.x;
        m_chunks.count = m_chunks.pitch * (def.rows / m_chunks.size.y);
    }

    // The global grid is allocated in one contiguous block, but will be broken down
    // as chunks and will not represent the global grid in memory.  For example,
    // say the chunk size is 2x2, and the layer size is 4x4.  The first four cells
    // will be from chunk[0], which on the global grid are indices 0, 1, 4, and 5.
    RDGE_CALLOC(m_chunks.data, m_chunks.count, nullptr);
    RDGE_CALLOC(m_cells.data, m_cells.count, nullptr);

    for (const auto& def_chunk : def.chunks)
    {
        size_t x = (def_chunk.x - def.start_x) / m_chunks.size.x;
        size_t y = (def_chunk.y - def.start_y) / m_chunks.size.y;
        size_t chunk_index = (y * m_chunks.pitch) + x;
        size_t grid_index = chunk_index * (m_chunks.size.w * m_chunks.size.h);

        auto& chunk = m_chunks.data[chunk_index];
        chunk.location = math::svec2(x, y);
        chunk.cells = &m_cells.data[grid_index];
        chunk.cell_count = m_chunks.size.w * m_chunks.size.h;

        math::vec2 origin = m_offset;
        origin.x += (m_cells.size.w * static_cast<float>(x * m_chunks.size.w));
        origin.y -= (m_cells.size.h * static_cast<float>(y * m_chunks.size.h));
        for (size_t i = 0; i < chunk.cell_count; i++)
        {
            if (def_chunk.data[i])
            {
                auto& cell = chunk.cells[i];
                cell.pos = origin;
                cell.pos.x += m_cells.size.w * static_cast<float>(i % m_chunks.size.w);
                cell.pos.y -= m_cells.size.h * static_cast<float>(i / m_chunks.size.w);

                uint32 gid = def_chunk.data[i] - 1;
                bool flip_x = (gid & FLIPPED_HORIZONTALLY);
                bool flip_y = (gid & FLIPPED_VERTICALLY);
                bool flip_d = (gid & FLIPPED_ANTIDIAGONALLY);
                gid &= ~(FLIPPED_HORIZONTALLY | FLIPPED_VERTICALLY | FLIPPED_ANTIDIAGONALLY);

                cell.uvs = tileset.tiles[gid];
                if (flip_x)
                {
                    cell.uvs.flip(TexCoordsFlip::HORIZONTAL);
                }

                if (flip_y)
                {
                    cell.uvs.flip(TexCoordsFlip::VERTICAL);
                }

                if (flip_d)
                {
                    cell.uvs.rotate(TexCoordsRotation::ROTATE_90);
                    cell.uvs.flip(TexCoordsFlip::VERTICAL);
                }
            }
        }
    }

    ILOG() << "TileLayer created:"
           << " tile_count=" << m_cells.count
           << " tile_size=" << m_cells.size
           << " chunk_count=" << m_chunks.count
           << " chunk_size=" << m_chunks.size
           << " offset=" << m_offset;
}

TileLayer::~TileLayer (void) noexcept
{
    RDGE_FREE(m_cells.data, nullptr);
    RDGE_FREE(m_chunks.data, nullptr);
}

TileLayer::TileLayer (TileLayer&& other) noexcept
    : m_cells(other.m_cells)
    , m_chunks(other.m_chunks)
    , m_offset(other.m_offset)
    , m_bounds(other.m_bounds)
    , m_color(other.m_color)
{
    other.m_cells.data = nullptr;
    other.m_chunks.data = nullptr;
}

TileLayer&
TileLayer::operator= (TileLayer&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_offset = rhs.m_offset;
        m_bounds = rhs.m_bounds;
        m_color = rhs.m_color;

        std::swap(m_cells, rhs.m_cells);
        std::swap(m_chunks, rhs.m_chunks);
    }

    return *this;
}

void
TileLayer::Draw (TileBatch& renderer, const OrthographicCamera& camera)
{
    // buffer the camera bounds by 5 tiles
    auto frame_bounds = camera.bounds;
    frame_bounds.fatten(m_cells.size.w * 5);

    if (!frame_bounds.intersects_with(m_bounds))
    {
        return;
    }

    renderer.SetView(camera);
    renderer.Prime();

    //if (frame_bounds.contains(m_bounds))
    {
        // draw everything
        for (size_t i = 0; i < m_chunks.count; i++)
        {
            renderer.Draw(m_chunks.data[i], m_color);
        }
    }
    //else
    //{
        //auto top_left = camera.bounds.top_left() - m_offset;
        //int32 x = top_left.x / m_chunks.size.w;
        //int32 y = top_left.y / m_chunks.size.h;
        //int32 chunk_index = (y * m_chunks.pitch) + x;
        //if (chunk_index >= 0 && chunk_index < (int32)m_chunks.count)
        //{
            //renderer.Draw(m_chunks.data[chunk_index], m_color);
        //}
    //}

    renderer.Flush();
}

std::ostream&
operator<< (std::ostream& os, TileRenderOrder value)
{
    return os << rdge::to_string(value);
}

std::string
to_string (TileRenderOrder value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(TileRenderOrder::INVALID)
        CASE(TileRenderOrder::RIGHT_DOWN)
        CASE(TileRenderOrder::RIGHT_UP)
        CASE(TileRenderOrder::LEFT_DOWN)
        CASE(TileRenderOrder::LEFT_UP)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

bool
try_parse (const std::string& test, TileRenderOrder& out)
{
    std::string s = rdge::to_lower(test);
    std::replace(s.begin(), s.end(), '-', '_');
    if      (s == "right_down") { out = TileRenderOrder::RIGHT_DOWN; return true; }
    else if (s == "right_up")   { out = TileRenderOrder::RIGHT_UP;   return true; }
    else if (s == "left_down")  { out = TileRenderOrder::LEFT_DOWN;  return true; }
    else if (s == "left_up")    { out = TileRenderOrder::LEFT_UP;    return true; }

    return false;
}

} // namespace rdge
