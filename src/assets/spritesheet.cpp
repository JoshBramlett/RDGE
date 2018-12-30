#include <rdge/assets/spritesheet.hpp>
#include <rdge/assets/pack_file.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/util/io/rwops_base.hpp>
#include <rdge/util/memory/alloc.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/json.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/debug/assert.hpp>

#include <exception>
#include <sstream>

namespace rdge {

using json = nlohmann::json;

namespace {

// Normalize the point to a float clamped to [0, 1]
constexpr float normalize (uint64 point, uint64 dimension)
{
    return static_cast<float>(point) / static_cast<float>(dimension);
}

void
ProcessAnimations (const json& j, SpriteSheet& sheet)
{
    if (j.count("animations") == 0)
    {
        return;
    }

    const auto& j_animations = j["animations"];
    std::vector<animation_data>(j_animations.size()).swap(sheet.animations);

    size_t index = 0;
    for (const auto& j_animation : j_animations)
    {
        JSON_VALIDATE_REQUIRED(j_animation, name, is_string);
        JSON_VALIDATE_REQUIRED(j_animation, mode, is_string);
        JSON_VALIDATE_REQUIRED(j_animation, frames, is_array);
        JSON_VALIDATE_OPTIONAL(j_animation, interval, is_number_unsigned);

        auto& animation = sheet.animations.at(index++);
        animation.name = j_animation["name"].get<decltype(animation.name)>();

        animation.value.interval = 0;
        if (j_animation.count("interval"))
        {
            animation.value.interval = j_animation["interval"].get<uint32>();
        }

        if (!try_parse(j_animation["mode"].get<std::string>(), animation.value.mode))
        {
            std::string msg("animation \"" + animation.name + "\" mode invalid");
            throw std::invalid_argument(msg);
        }

        const auto& j_frames = j_animation["frames"];
        for (const auto& j_frame : j_frames)
        {
            JSON_VALIDATE_REQUIRED(j_frame, name, is_string);
            JSON_VALIDATE_OPTIONAL(j_frame, flip, is_number_unsigned);

            auto frame_name = j_frame["name"].get<std::string>();
            TexCoordsFlip frame_flip = TexCoordsFlip::NONE;
            if (j_frame.count("flip"))
            {
                frame_flip = static_cast<TexCoordsFlip>(j_frame["flip"].get<uint32>());
            }

            animation.value.frames.reserve(j_frames.size());
            bool found = false;
            for (const auto& region : sheet.regions)
            {
                if (region.name == frame_name)
                {
                    auto region_copy = region.value;
                    region_copy.flip(frame_flip);

                    animation_frame frame;
                    frame.size = region_copy.sprite_size;
                    frame.origin = region_copy.sprite_size;
                    frame.origin.x *= region_copy.origin.x;
                    frame.origin.y *= region_copy.origin.y;
                    frame.uvs = region_copy.coords;

                    animation.value.frames.push_back(frame);

                    found = true;
                    break;
                }
            }

            if (!found)
            {
                std::ostringstream ss;
                ss << "animation \"" << animation.name << "\" cannot find "
                   << "frame \"" << frame_name << "\" in region list";
                throw std::invalid_argument(ss.str());
            }
        }
    }
}

void
ProcessSlices (const json& j, SpriteSheet& sheet)
{
    if (j.count("slices") == 0)
    {
        return;
    }

    const auto& j_slices = j["slices"];
    std::vector<slice_data>(j_slices.size()).swap(sheet.slices);

    size_t index = 0;
    for (const auto& j_slice : j_slices)
    {
        JSON_VALIDATE_REQUIRED(j_slice, name, is_string);
        JSON_VALIDATE_REQUIRED(j_slice, color, is_string);
        JSON_VALIDATE_REQUIRED(j_slice, keys, is_array);
        JSON_VALIDATE_OPTIONAL(j_slice, data, is_string);

        auto& slice = sheet.slices.at(index++);
        slice.name = j_slice["name"].get<decltype(slice.name)>();
        slice.color = color::from_rgba(j_slice["color"].get<std::string>());

        const auto& j_keys = j_slice["keys"];
        RDGE_ASSERT(j_keys.size() == 1u);

        const auto& j_key = j_keys.at(0);
        JSON_VALIDATE_REQUIRED(j_key, frame, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j_key, bounds, is_object);
        JSON_VALIDATE_OPTIONAL(j_key, center, is_object);

        auto frame_index = j_key["frame"].get<size_t>();
        auto& region = sheet.regions.at(frame_index);

        auto bounds = j_key["bounds"].get<screen_rect>();
        slice.bounds.x = region.value.clip.x + bounds.x;
        slice.bounds.y = region.value.clip.y + bounds.y;
        slice.bounds.w = bounds.w;
        slice.bounds.h = bounds.h;

        slice.is_nine_patch = !!j_key.count("center");
        if (slice.is_nine_patch)
        {
            auto center = j_key["center"].get<screen_rect>();
            slice.center.x = center.x;
            slice.center.y = center.y;
            slice.center.w = center.w;
            slice.center.h = center.h;
        }
    }
}

void
ProcessSpriteSheet (const json& j, SpriteSheet& sheet)
{
    JSON_VALIDATE_REQUIRED(j, frames, is_array);
    JSON_VALIDATE_REQUIRED(j, meta, is_object);
    JSON_VALIDATE_OPTIONAL(j, animations, is_array);

    const auto& j_regions = j["frames"];
    std::vector<region_data>(j_regions.size()).swap(sheet.regions);

    auto surface_size = sheet.surface->Size();
    size_t index = 0;
    for (const auto& j_region : j_regions)
    {
        JSON_VALIDATE_REQUIRED(j_region, filename, is_string);
        JSON_VALIDATE_REQUIRED(j_region, rotated, is_boolean);
        JSON_VALIDATE_REQUIRED(j_region, trimmed, is_boolean);
        JSON_VALIDATE_REQUIRED(j_region, frame, is_object);
        JSON_VALIDATE_REQUIRED(j_region, sourceSize, is_object);
        JSON_VALIDATE_REQUIRED(j_region, spriteSourceSize, is_object);

        JSON_VALIDATE_OPTIONAL(j_region, pivot, is_object);

        // objectsheet only
        JSON_VALIDATE_OPTIONAL(j_region, index, is_number_unsigned);
        JSON_VALIDATE_OPTIONAL(j_region, objects, is_array);
        JSON_VALIDATE_OPTIONAL(j_region, type, is_string);

        // override index if provided through the config
        if (j_region.count("index"))
        {
            index = j_region["index"].get<size_t>();
        }

        auto& region = sheet.regions.at(index++);
        region.name = j_region["filename"].get<std::string>();
        region.value.is_rotated = j_region["rotated"].get<bool>();

        const auto& j_frame = j_region["frame"];
        JSON_VALIDATE_REQUIRED(j_frame, x, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j_frame, y, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j_frame, w, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j_frame, h, is_number_unsigned);
        region.value.clip = { j_frame["x"].get<int32>(),
                              j_frame["y"].get<int32>(),
                              j_frame["w"].get<int32>(),
                              j_frame["h"].get<int32>() };
        if (region.value.is_rotated)
        {
            std::swap(region.value.clip.w, region.value.clip.h);
        }

        // Validate values are within range
        if ((region.value.clip.x + region.value.clip.w > (int32)surface_size.w) ||
            (region.value.clip.y + region.value.clip.h > (int32)surface_size.h))
        {
            std::string msg("region \"" + region.name + "\" outside valid range");
            throw std::invalid_argument(msg);
        }

        float x1 = normalize(region.value.clip.x, surface_size.w);
        float x2 = normalize(region.value.clip.x + region.value.clip.w, surface_size.w);
        float y1 = normalize(region.value.clip.y, surface_size.h);
        float y2 = normalize(region.value.clip.y + region.value.clip.h, surface_size.h);
        region.value.coords.bottom_left  = math::vec2(x1, y1);
        region.value.coords.bottom_right = math::vec2(x2, y1);
        region.value.coords.top_left     = math::vec2(x1, y2);
        region.value.coords.top_right    = math::vec2(x2, y2);

        if (region.value.is_rotated)
        {
            // rotation must be done prior to the size/pivot being set, as the
            // values from the TexturePacker export are not updated post rotation
            region.value.rotate(TexCoordsRotation::ROTATE_270);
        }

        {
            const auto& j_ss = j_region["sourceSize"];
            JSON_VALIDATE_REQUIRED(j_ss, w, is_number_unsigned);
            JSON_VALIDATE_REQUIRED(j_ss, h, is_number_unsigned);

            auto& sz = region.value.size;
            sz.w = j_ss["w"].get<decltype(sz.w)>();
            sz.h = j_ss["h"].get<decltype(sz.h)>();
        }

        {
            const auto& j_sss = j_region["spriteSourceSize"];
            JSON_VALIDATE_REQUIRED(j_sss, x, is_number_unsigned);
            JSON_VALIDATE_REQUIRED(j_sss, y, is_number_unsigned);
            JSON_VALIDATE_REQUIRED(j_sss, w, is_number_unsigned);
            JSON_VALIDATE_REQUIRED(j_sss, h, is_number_unsigned);

            auto& sz = region.value.sprite_size;
            sz.w = j_sss["w"].get<decltype(sz.w)>();
            sz.h = j_sss["h"].get<decltype(sz.h)>();

            // invert offset to convert to y-is-up
            auto& offset = region.value.sprite_offset;
            offset.x = j_sss["x"].get<decltype(offset.x)>();
            offset.y = region.value.size.h - (sz.h + j_sss["y"].get<decltype(offset.y)>());
        }

        if (j_region.count("pivot"))
        {
            const auto& j_pivot = j_region["pivot"];
            JSON_VALIDATE_REQUIRED(j_pivot, x, is_number);
            JSON_VALIDATE_REQUIRED(j_pivot, y, is_number);

            // convert pivot/origin normal to y-is-up
            auto& origin = region.value.origin;
            origin.x = j_pivot["x"].get<decltype(origin.x)>();
            origin.y = (1.f - j_pivot["y"].get<decltype(origin.y)>());
        }
        else
        {
            // no specified pivot defaults to the center
            auto& origin = region.value.origin;
            origin.x = 0.5f;
            origin.y = 0.5f;
        }

        if (j_region.count("type"))
        {
            region.type = j_region["type"].get<decltype(region.type)>();
        }

        if (j_region.count("objects"))
        {
            region.objects.reserve(j_region["objects"].size());
            for (const auto& j_obj : j_region["objects"])
            {
                region.objects.emplace_back(j_obj);

                // The object has relative position to the sprite, so it's y-axis
                // must be positive on import.  The object ctor negates the y-axis,
                // so we can safely assume a negative value.
                auto& obj = region.objects.back();
                RDGE_ASSERT(obj.pos.y <= 0.f);
                RDGE_ASSERT(obj.type == tilemap::ObjectType::POLYGON ||
                            obj.type == tilemap::ObjectType::CIRCLE);

                obj.pos.y = region.value.sprite_size.h + obj.pos.y;
            }
        }
    }

    const auto& j_meta = j["meta"];
    JSON_VALIDATE_OPTIONAL(j_meta, animations, is_array);
    JSON_VALIDATE_OPTIONAL(j_meta, slices, is_array);

    ProcessAnimations(j, sheet);
    ProcessSlices(j_meta, sheet);
}

} // anonymous namespace

void
spritesheet_region::flip (TexCoordsFlip f) noexcept
{
    coords.flip(f);

    if (f == TexCoordsFlip::HORIZONTAL)
    {
        origin.x = 1.f - origin.x;
    }
    else if (f == TexCoordsFlip::VERTICAL)
    {
        origin.y = 1.f - origin.y;
    }
}

void
spritesheet_region::rotate (TexCoordsRotation r) noexcept
{
    coords.rotate(r);

    switch (r)
    {
    case TexCoordsRotation::ROTATE_90:
        origin = math::vec2(origin.y, 1.f - origin.x);
        size = math::vec2(size.h, size.w);
        sprite_size = math::vec2(sprite_size.h, sprite_size.w);

        break;
    case TexCoordsRotation::ROTATE_180:
        origin = math::vec2(1.f - origin.x, 1.f - origin.y);

        break;
    case TexCoordsRotation::ROTATE_270:
        origin = math::vec2(1.f - origin.y, origin.x);
        size = math::vec2(size.h, size.w);
        sprite_size = math::vec2(sprite_size.h, sprite_size.w);

        break;
    case TexCoordsRotation::NONE:
    default:
        break;
    }
}

void
spritesheet_region::scale (float scale) noexcept
{
    size *= scale;
    sprite_offset *= scale;
    sprite_size *= scale;
}

SpriteSheet::SpriteSheet (const char* filepath)
{
    try
    {
        const auto j = json::parse(GetTextFileContent(filepath));
        JSON_VALIDATE_REQUIRED(j, meta, is_object);

        const auto& j_meta = j["meta"];
        JSON_VALIDATE_REQUIRED(j_meta, image, is_string);

        void* pnew = RDGE_MALLOC(sizeof(Surface), memory_bucket_assets);
        if (RDGE_UNLIKELY(!pnew))
        {
            throw std::runtime_error("Memory allocation failed");
        }

        Surface* raw = new (pnew) Surface(j_meta["image"].get<std::string>());
        this->surface = shared_asset<Surface>(raw);
        ProcessSpriteSheet(j, *this);
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

SpriteSheet::SpriteSheet (const std::vector<uint8>& msgpack, PackFile& packfile)
{
    try
    {
        json j = json::from_msgpack(msgpack);
        JSON_VALIDATE_REQUIRED(j, meta, is_object);

        const auto& j_meta = j["meta"];
        JSON_VALIDATE_REQUIRED(j_meta, image_table_id, is_number);

        this->surface = packfile.GetAsset<Surface>(j_meta["image_table_id"].get<int32>());
        ProcessSpriteSheet(j, *this);
    }
    catch (const std::exception& ex)
    {
        RDGE_THROW(ex.what());
    }
}

const spritesheet_region&
SpriteSheet::operator[] (const std::string& name) const
{
    for (const auto& region : this->regions)
    {
        if (region.name == name)
        {
            return region.value;
        }
    }

    RDGE_THROW("SpriteSheet region lookup failed. key=" + name);
}

Animation
SpriteSheet::GetAnimation (const std::string& name, float scale) const
{
    for (const auto& animation : this->animations)
    {
        if (animation.name == name)
        {
            auto result = animation.value;
            for (auto& frame : result.frames)
            {
                frame.size *= scale;
                frame.origin *= scale;
            }

            return result;
        }
    }

    RDGE_THROW("SpriteSheet animation lookup failed. key=" + name);
}

Animation
SpriteSheet::GetAnimation (int32 animation_id, float scale) const
{
    RDGE_ASSERT(animation_id >= 0);
    RDGE_ASSERT(static_cast<size_t>(animation_id) < this->animations.size());

    auto result = this->animations[animation_id].value;
    for (auto& frame : result.frames)
    {
        frame.size *= scale;
        frame.origin *= scale;
    }

    return result;
}

std::ostream& operator<< (std::ostream& os, const spritesheet_region& p)
{
    os << "spritesheet_region: ["
       << "\n  clip=" << p.clip
       << "\n  coords=" << p.coords
       << "\n  size=" << p.size
       << "\n  origin=" << p.origin
       << "\n]\n";

    return os;
}

} // namespace rdge
