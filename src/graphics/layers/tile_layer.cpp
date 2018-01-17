#include <rdge/graphics/layers/tile_layer.hpp>
#include <rdge/assets/tileset.hpp>
#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/renderers/tile_batch.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/memory/alloc.hpp>

#include <SDL_assert.h>

#include <sstream>
#include <algorithm>

namespace rdge {

namespace {

constexpr uint32 FLIPPED_HORIZONTALLY   = 0x80000000;
constexpr uint32 FLIPPED_VERTICALLY     = 0x40000000;
constexpr uint32 FLIPPED_ANTIDIAGONALLY = 0x20000000;

} // anonymous namespace

TileLayer::TileLayer (const tilemap_grid& grid,
                      const tilemap::Layer& def,
                      const Tileset& tileset,
                      float scale)
    : m_grid(grid)
    , m_offset(def.offset * scale)
    , texture(std::make_shared<Texture>(tileset.surface))
{
    // Convert to y-is-up
    m_offset.y *= -1.f;
    m_grid.pos.y *= -1;
    m_grid.cell_size *= scale;

    math::vec2 pixel_offset = m_offset;
    m_offset.x += static_cast<float>(m_grid.cell_size.w) * m_grid.pos.x;
    m_offset.y += static_cast<float>(m_grid.cell_size.h) * m_grid.pos.y;

    math::vec2 lo = m_offset;
    math::vec2 hi = m_offset;
    lo.y -= static_cast<float>(m_grid.cell_size.h) * m_grid.size.h;
    hi.x += static_cast<float>(m_grid.cell_size.w) * m_grid.size.w;
    m_bounds = physics::aabb(lo, hi);

    m_color.a = 255.f * def.opacity;

    m_inv.w = 1.f / (m_grid.cell_size.w * m_grid.chunk_size.w);
    m_inv.h = 1.f / (m_grid.cell_size.h * m_grid.chunk_size.h);

    m_chunks.rows = m_grid.size.h / m_grid.chunk_size.h;
    m_chunks.cols = m_grid.size.w / m_grid.chunk_size.w;
    m_chunks.count = m_chunks.rows * m_chunks.cols;

    // The global grid is broken down into fixed sized chunks, but not all chunks
    // may be represented in memory.  If there are no renderable tiles in a chunk
    // it will be omitted in the definition.
    //
    // Therefore, rather than allocating space for all cells in the grid, we only
    // allocate enough cells for the valid chunks.  The cells are allocated in one
    // contiguous block, but simply assigned to the chunks for use.
    //
    // All chunks in the grid are allocated, but if there is no corresponding definition
    // the cell data for that chunk will be null.
    size_t cells_in_chunk = m_grid.chunk_size.w * m_grid.chunk_size.h;
    RDGE_CALLOC(m_chunks.data, m_chunks.count, nullptr);
    RDGE_CALLOC(m_cells, cells_in_chunk * def.chunks.size(), nullptr);

    size_t cells_index = 0;
    for (const auto& def_chunk : def.chunks)
    {
        SDL_assert(cells_in_chunk == def_chunk.data.size());

        // x/y in local chunk coordinates
        int32 chunk_x = (def_chunk.x - grid.pos.x) / m_grid.chunk_size.w;
        int32 chunk_y = (def_chunk.y - grid.pos.y) / m_grid.chunk_size.h;
        SDL_assert(chunk_x >= 0);
        SDL_assert(chunk_y >= 0);

        size_t chunk_index = (chunk_y * m_chunks.cols) + chunk_x;
        auto& chunk = m_chunks.data[chunk_index];
        chunk.cell_count = cells_in_chunk;
        chunk.cells = &m_cells[cells_index];
        cells_index += cells_in_chunk;

        math::vec2 origin = pixel_offset;
        origin.x += static_cast<float>(m_grid.cell_size.w) * def_chunk.x;
        origin.y -= static_cast<float>(m_grid.cell_size.h) * def_chunk.y;
        for (size_t i = 0; i < chunk.cell_count; i++)
        {
            if (def_chunk.data[i])
            {
                auto& cell = chunk.cells[i];
                cell.pos = origin;
                cell.pos.x += m_grid.cell_size.w * static_cast<float>(i % m_grid.chunk_size.w);
                cell.pos.y -= m_grid.cell_size.h * static_cast<float>(i / m_grid.chunk_size.w);

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
           << " chunks=" << m_chunks.count
           << " pitch=" << m_chunks.cols
           << " offset=" << m_offset;
}

TileLayer::~TileLayer (void) noexcept
{
    RDGE_FREE(m_cells, nullptr);
    RDGE_FREE(m_chunks.data, nullptr);
}

TileLayer::TileLayer (TileLayer&& other) noexcept
    : m_grid(other.m_grid)
    , m_cells(other.m_cells)
    , m_chunks(other.m_chunks)
    , m_offset(other.m_offset)
    , m_bounds(other.m_bounds)
    , m_color(other.m_color)
    , m_inv(other.m_inv)
{
    other.m_cells = nullptr;
    other.m_chunks.data = nullptr;
}

TileLayer&
TileLayer::operator= (TileLayer&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_grid = rhs.m_grid;
        m_offset = rhs.m_offset;
        m_bounds = rhs.m_bounds;
        m_color = rhs.m_color;
        m_inv = rhs.m_inv;

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
    frame_bounds.fatten(m_grid.cell_size.w * 2.f);

    if (!frame_bounds.intersects_with(m_bounds))
    {
        return;
    }

    renderer.SetView(camera);
    renderer.Prime();

    float left = (camera.bounds.left() - m_bounds.left());
    float right = left + camera.bounds.width();
    int32 x1 = std::max(static_cast<int32>(left * m_inv.w), 0);
    int32 x2 = std::min(static_cast<int32>((right * m_inv.w) + 1.f),
                        static_cast<int32>(m_chunks.cols));

    float top = (m_bounds.top() - camera.bounds.top());
    float bottom = top + camera.bounds.height();
    int32 y1 = std::max(static_cast<int32>(top * m_inv.h), 0);
    int32 y2 = std::min(static_cast<int32>((bottom * m_inv.h) + 1.f),
                        static_cast<int32>(m_chunks.rows));

    //size_t draw_calls = 0;
    for (int32 col = x1; col < x2; col++)
    {
        for (int32 row = y1; row < y2; row++)
        {
            size_t chunk_index = (row * m_chunks.cols) + col;
            if (m_chunks.data[chunk_index].cells)
            {
                renderer.Draw(m_chunks.data[chunk_index], m_color);
                //draw_calls++;
            }
        }
    }

    //ILOG() << "this=" << this << " draw calls: " << draw_calls;

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
