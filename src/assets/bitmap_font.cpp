#include <rdge/assets/bitmap_font.hpp>
#include <rdge/assets/pack_file.hpp>
#include <rdge/assets/file_formats/bmfont.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <SDL_assert.h>

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
    try
    {
        bmfont_data font;
        load_bmfont(filepath, font);

        this->name = font.info.face;
        this->size = static_cast<size_t>(font.info.size);
        this->line_height = static_cast<float>(font.common.lineHeight);
        this->baseline = static_cast<float>(font.common.base);
        m_flags = font.info.flags;

        SDL_assert(font.common.pages > 0);
        std::vector<shared_asset<Surface>>(font.common.pages).swap(this->surfaces);

        std::string dirname = rdge::dirname(filepath);
        if (!dirname.empty())
        {
            dirname += '/';
        }

        for (const auto& page : font.pages)
        {
            void* pnew = RDGE_MALLOC(sizeof(Surface), memory_bucket_assets);
            if (RDGE_UNLIKELY(!pnew))
            {
                throw std::invalid_argument("Memory allocation failed");
            }

            Surface* raw = new (pnew) Surface(dirname + page.file);
            if (raw->Width() != font.common.scaleW || raw->Height() != font.common.scaleH)
            {
                throw std::invalid_argument("Surface size doesn't match font info");
            }

            this->surfaces.at(page.id) = shared_asset<Surface>(raw);
        }

        SDL_assert(font.high_id > 0);
        std::vector<glyph_region>(font.high_id + 1).swap(this->glyphs);

        for (const auto& c : font.chars)
        {
            SDL_assert(c.id < this->glyphs.size());
            SDL_assert(c.page < this->surfaces.size());
            auto& g = this->glyphs.at(c.id);
            g.id = c.id;
            g.clip = { static_cast<int32>(c.x), static_cast<int32>(c.y),
                       static_cast<int32>(c.width), static_cast<int32>(c.height) };
            g.size = { static_cast<float>(c.width), static_cast<float>(c.height) };
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
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

BitmapFont::BitmapFont (const std::vector<uint8>& msgpack, PackFile& packfile)
{

}

const glyph_region&
BitmapFont::operator[] (uint32 id) const
{
    try
    {
        return this->glyphs.at(id);
    }
    catch (const std::out_of_range&)
    {
        RDGE_THROW("BitmapFont lookup failed. key=" + std::to_string(id));
    }
}

bool
BitmapFont::IsUnicode (void) const noexcept
{
    return (m_flags & bmfont_info_unicode);
}

bool
BitmapFont::IsBold (void) const noexcept
{
    return (m_flags & bmfont_info_bold);
}

bool
BitmapFont::IsItalic (void) const noexcept
{
    return (m_flags & bmfont_info_italic);
}

bool
BitmapFont::IsDistanceField (void) const noexcept
{
    return (this->distance_field.spread > 0.f);
}

} // namespace rdge
