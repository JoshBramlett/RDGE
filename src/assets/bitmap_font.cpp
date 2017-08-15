#include <rdge/assets/bitmap_font.hpp>
#include <rdge/assets/bmfont.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <SDL_assert.h>

#include <exception>

// TODO THIS IS UNTESTED

namespace {

using namespace rdge;

// Normalize the point to a float clamped to [0, 1]
constexpr float
normalize (uint64 point, uint64 dimension)
{
    return static_cast<float>(point) / static_cast<float>(dimension);
}

} // anonymous namespace

namespace rdge {

BitmapFont::BitmapFont (const char* filepath)
{
    bmfont_data font;
    load_bmfont(filepath, font);

    this->pad_top = static_cast<float>(font.info.padding[0]);
    this->pad_right = static_cast<float>(font.info.padding[1]);
    this->pad_bottom = static_cast<float>(font.info.padding[2]);
    this->pad_left = static_cast<float>(font.info.padding[3]);
    this->line_height = static_cast<float>(font.common.lineHeight);

    SDL_assert(font.common.pages > 0);
    this->surfaces.reserve(font.common.pages);
    this->textures.reserve(font.common.pages);

    for (const auto& page : font.pages)
    {
        auto& s = this->surfaces.at(page.id);
        s = std::make_shared<Surface>(page.file);

        auto& t = this->textures.at(page.id);
        t = std::make_shared<Texture>(*s.get());
    }

    SDL_assert(font.high_id > 0);
    m_glyphs.reserve(font.high_id);

    for (const auto& c : font.chars)
    {
        auto& g = m_glyphs.at(c.id);
        g.id = c.id;
        g.clip = { static_cast<int32>(c.x), static_cast<int32>(c.y),
                   static_cast<int32>(c.width), static_cast<int32>(c.height) };
        g.size = { c.width, c.height };
        g.page = c.page;

        auto surface_size = this->surfaces.at(c.page)->Size();
        g.coords.bottom_left  = math::vec2(::normalize(c.x, surface_size.w),
                                           ::normalize(c.y, surface_size.h));
        g.coords.bottom_right = math::vec2(::normalize(c.x + c.width, surface_size.w),
                                           ::normalize(c.y, surface_size.h));
        g.coords.top_left     = math::vec2(::normalize(c.x, surface_size.w),
                                           ::normalize(c.y + c.height, surface_size.h));
        g.coords.top_right    = math::vec2(::normalize(c.x + c.width, surface_size.w),
                                           ::normalize(c.y + c.height, surface_size.h));

        g.offset = { static_cast<float>(c.xoffset), static_cast<float>(c.yoffset) };
        g.x_advance = static_cast<float>(c.xadvance);
    }
}

const glyph&
BitmapFont::operator[] (uint32 id) const
{
    try {
        return m_glyphs.at(id);
    } catch (const std::out_of_range& ex) {
        RDGE_THROW("BitmapFont lookup failed. key=" + std::to_string(id));
    }
}

} // namespace rdge
