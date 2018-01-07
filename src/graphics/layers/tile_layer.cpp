#include <rdge/graphics/layers/tile_layer.hpp>
#include <rdge/graphics/isprite.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/renderers/sprite_batch.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/internal/hints.hpp>

#include <SDL_assert.h>

#include <algorithm>

namespace rdge {

namespace {

constexpr uint32 FLIPPED_HORIZONTALLY   = 0x80000000;
constexpr uint32 FLIPPED_VERTICALLY     = 0x40000000;
constexpr uint32 FLIPPED_ANTIDIAGONALLY = 0x20000000;

} // anonymous namespace

TileLayer::TileLayer (const tilemap::Layer& def, const Tileset& tileset, float scale)
    : cell_count(def.rows * def.cols)
    , cell_pitch(def.cols)
    , cell_size(tileset.tile_size * scale)
    , offset(def.offset * scale)
    , opacity(def.opacity)
    , texture(std::make_shared<Texture>(tileset.surface))
{
    this->offset.x += this->cell_size.x * static_cast<float>(def.start_x);
    this->offset.y += this->cell_size.y * static_cast<float>(def.start_y);

    // convert to y-is-up
    this->offset.y = -this->offset.y;

    RDGE_CALLOC(this->cells, this->cell_count, nullptr);
    for (size_t i = 0; i < this->cell_count; i++)
    {
        auto& cell = this->cells[i];
        cell.pos = this->offset;
        cell.pos.x += this->cell_size.x * static_cast<float>(i % cell_pitch);
        cell.pos.y -= this->cell_size.y * static_cast<float>(i / cell_pitch);

        uint32 gid = def.data[i];
        if (gid >= 0)
        {
            bool flip_x = (gid & FLIPPED_HORIZONTALLY);
            bool flip_y = (gid & FLIPPED_VERTICALLY);
            bool flip_d = (gid & FLIPPED_ANTIDIAGONALLY);
            gid &= ~(FLIPPED_HORIZONTALLY | FLIPPED_VERTICALLY | FLIPPED_ANTIDIAGONALLY);

            cell.coords = tileset.tiles[gid];
            if (flip_x)
            {
                cell.coords.flip(TexCoordsFlip::HORIZONTAL);
            }

            if (flip_y)
            {
                cell.coords.flip(TexCoordsFlip::VERTICAL);
            }

            if (flip_d)
            {
                cell.coords.rotate(TexCoordsRotation::ROTATE_90);
                cell.coords.flip(TexCoordsFlip::VERTICAL);
            }
        }
    }
}

TileLayer::~TileLayer (void) noexcept
{
    RDGE_FREE(this->cells, nullptr);
}

void
TileLayer::Draw (void)
{
    this->renderer->PrepSubmit();

    for (const auto& sprite : this->sprites)
    {
        sprite->Draw(*this->renderer);
    }

    this->renderer->Flush();
}

std::ostream&
operator<< (std::ostream& os, RenderOrder value)
{
    return os << rdge::to_string(value);
}

std::string
to_string (tilemap::RenderOrder value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(RenderOrder::INVALID)
        CASE(RenderOrder::RIGHT_DOWN)
        CASE(RenderOrder::RIGHT_UP)
        CASE(RenderOrder::LEFT_DOWN)
        CASE(RenderOrder::LEFT_UP)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

bool
try_parse (const std::string& test, RenderOrder& out)
{
    std::string s = rdge::to_lower(test);
    std::replace(s.begin(), s.end(), '-', '_');
    if      (s == "right_down") { out = RenderOrder::RIGHT_DOWN; return true; }
    else if (s == "right_up")   { out = RenderOrder::RIGHT_UP;   return true; }
    else if (s == "left_down")  { out = RenderOrder::LEFT_DOWN;  return true; }
    else if (s == "left_up")    { out = RenderOrder::LEFT_UP;    return true; }

    return false;
}

} // namespace rdge
