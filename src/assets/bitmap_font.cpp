#include <rdge/assets/bitmap_font.hpp>
#include <rdge/assets/pack_file.hpp>
#include <rdge/assets/file_formats/bmfont.hpp>
#include <rdge/util/json.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/strings.hpp>

#include <SDL_assert.h>

namespace rdge {

using json = nlohmann::json;

namespace {

// Normalize the point to a float clamped to [0, 1]
constexpr float
normalize (uint64 point, uint64 dimension)
{
    return static_cast<float>(point) / static_cast<float>(dimension);
}

void
ProcessBitmapFont (const bmfont_data& data, BitmapFont& font)
{
    font.name = data.info.face;
    font.size = static_cast<size_t>(data.info.size);
    font.line_height = static_cast<float>(data.common.lineHeight);
    font.baseline = static_cast<float>(data.common.base);

    SDL_assert(data.high_id > 0);
    std::vector<glyph_region>(data.high_id + 1).swap(font.glyphs);

    for (const auto& c : data.chars)
    {
        SDL_assert(c.id < font.glyphs.size());
        SDL_assert(c.page < font.surfaces.size());
        auto& g = font.glyphs.at(c.id);
        g.id = c.id;
        g.clip = { static_cast<int32>(c.x), static_cast<int32>(c.y),
                   static_cast<int32>(c.width), static_cast<int32>(c.height) };
        g.size = { static_cast<float>(c.width), static_cast<float>(c.height) };
        g.page = c.page;

        auto surface_size = font.surfaces.at(c.page)->Size();
        g.coords.bottom_left  = math::vec2(normalize(c.x, surface_size.w),
                                           normalize(c.y, surface_size.h));
        g.coords.bottom_right = math::vec2(normalize(c.x + c.width, surface_size.w),
                                           normalize(c.y, surface_size.h));
        g.coords.top_left     = math::vec2(normalize(c.x, surface_size.w),
                                           normalize(c.y + c.height, surface_size.h));
        g.coords.top_right    = math::vec2(normalize(c.x + c.width, surface_size.w),
                                           normalize(c.y + c.height, surface_size.h));

        g.offset = { static_cast<float>(c.xoffset), static_cast<float>(c.yoffset) };
        g.x_advance = static_cast<float>(c.xadvance);
    }
}

} // anonymous namespace

BitmapFont::BitmapFont (const char* filepath)
{
    try
    {
        bmfont_data font;
        load_bmfont(filepath, font);

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

        ProcessBitmapFont(font, *this);
        m_flags = font.info.flags;
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

BitmapFont::BitmapFont (const std::vector<uint8>& msgpack, PackFile& packfile)
{
    try
    {
        json j = json::from_msgpack(msgpack);
        bmfont_data font;
        load_bmfont(j, font);

        this->surfaces.reserve(font.pages.size());
        for (const auto& page : font.pages)
        {
            this->surfaces.emplace_back(packfile.GetAsset<Surface>(page.image_table_id));
        }

        ProcessBitmapFont(font, *this);
        m_flags = font.info.flags;

        if (j.count("effects"))
        {
            const auto& j_effects = j["effects"];
            for (const auto& j_effect : j_effects)
            {
                auto effect_name = j_effect["class"].get<std::string>();
                if (effect_name == "DistanceFieldEffect")
                {
                    this->distance_field.color = color::from_rgb(j_effect["color"].get<std::string>());
                    this->distance_field.scale = j_effect["scale"].get<size_t>();
                    this->distance_field.spread = j_effect["spread"].get<float>();
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
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
