#include <rdge/assets/spritesheet.hpp>
#include <rdge/assets/pack_file.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/util/io/rwops_base.hpp>
#include <rdge/util/json.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <SDL_assert.h>

#include <exception>
#include <sstream>

namespace rdge {

using namespace rdge::math;
using json = nlohmann::json;

namespace {

// Normalize the point to a float clamped to [0, 1]
constexpr float normalize (uint64 point, uint64 dimension)
{
    return static_cast<float>(point) / static_cast<float>(dimension);
}

void
ProcessSpriteSheet (const json& j, SpriteSheet& sheet)
{
    JSON_VALIDATE_REQUIRED(j, frames, is_array);
    JSON_VALIDATE_OPTIONAL(j, animations, is_array);

    const auto& j_regions = j["frames"];
    sheet.regions = std::vector<region_data>(j_regions.size());

    auto surface_size = sheet.surface.Size();
    size_t index = 0;
    for (const auto& j_region : j_regions)
    {
        JSON_VALIDATE_REQUIRED(j_region, filename, is_string);
        JSON_VALIDATE_REQUIRED(j_region, rotated, is_boolean);
        JSON_VALIDATE_REQUIRED(j_region, trimmed, is_boolean);
        JSON_VALIDATE_REQUIRED(j_region, frame, is_object);
        JSON_VALIDATE_REQUIRED(j_region, pivot, is_object);
        JSON_VALIDATE_REQUIRED(j_region, sourceSize, is_object);
        JSON_VALIDATE_REQUIRED(j_region, spriteSourceSize, is_object);

        // objectsheet only
        JSON_VALIDATE_OPTIONAL(j_region, index, is_number_unsigned);
        JSON_VALIDATE_OPTIONAL(j_region, objects, is_array);

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
        region.value.coords.bottom_left  = vec2(x1, y1);
        region.value.coords.bottom_right = vec2(x2, y1);
        region.value.coords.top_left     = vec2(x1, y2);
        region.value.coords.top_right    = vec2(x2, y2);

        if (region.value.is_rotated)
        {
            // rotation must be done prior to the size/pivot being set, as the
            // values from the TexturePacker export are not updated post rotation
            region.value.rotate(TexCoordsRotation::ROTATE_270);
        }

        const auto& j_size = j_region["sourceSize"];
        JSON_VALIDATE_REQUIRED(j_size, w, is_number_unsigned);
        JSON_VALIDATE_REQUIRED(j_size, h, is_number_unsigned);
        region.value.size.w = j_size["w"].get<float>();
        region.value.size.h = j_size["h"].get<float>();

        // convert pivot to pixels and y-is-up
        const auto& j_pivot = j_region["pivot"];
        JSON_VALIDATE_REQUIRED(j_pivot, x, is_number_float);
        JSON_VALIDATE_REQUIRED(j_pivot, y, is_number_float);
        region.value.origin.x = region.value.size.w * j_pivot["x"].get<float>();
        region.value.origin.y = region.value.size.h * (1.f - j_pivot["y"].get<float>());

        if (j_region.count("objects"))
        {
            region.objects.reserve(j_region["objects"].size());
            for (const auto& j_obj : j_region["objects"])
            {
                region.objects.emplace_back(tilemap::Object(j_obj));
            }
        }
    }

    if (j.count("animations"))
    {
        const auto& j_animations = j["animations"];
        sheet.animations = std::vector<animation_data>(j_animations.size());

        index = 0;
        for (const auto& j_animation : j_animations)
        {
            JSON_VALIDATE_REQUIRED(j_animation, name, is_string);
            JSON_VALIDATE_REQUIRED(j_animation, interval, is_number_unsigned);
            JSON_VALIDATE_REQUIRED(j_animation, mode, is_string);
            JSON_VALIDATE_REQUIRED(j_animation, frames, is_array);

            auto& animation = sheet.animations.at(index++);
            animation.name = j_animation["name"].get<std::string>();
            animation.value.interval = j_animation["interval"].get<uint32>();

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

                bool found = false;
                for (const auto& region : sheet.regions)
                {
                    if (region.name == frame_name)
                    {
                        auto region_copy = region.value;
                        region_copy.flip(frame_flip);
                        animation.value.frames.emplace_back(region_copy);

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
}

} // anonymous namespace

SpriteSheet::SpriteSheet (const char* filepath)
{
    try
    {
        auto rwops = rwops_base::from_file(filepath, "rt");
        auto sz = rwops.size();

        std::string file_data(sz + 1, '\0');
        rwops.read(file_data.data(), sizeof(char), sz);

        const auto j = json::parse(file_data);
        JSON_VALIDATE_REQUIRED(j, meta, is_object);

        const auto& j_meta = j["meta"];
        JSON_VALIDATE_REQUIRED(j_meta, image, is_string);

        this->surface = Surface(j_meta["image"].get<std::string>());
        ProcessSpriteSheet(j, *this);
    }
    catch (const std::logic_error& ex)
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

        this->surface = packfile.GetSurface(j_meta["image_table_id"].get<int32>());
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
                frame.scale(scale);
            }

            return result;
        }
    }

    RDGE_THROW("SpriteSheet animation lookup failed. key=" + name);
}

Animation
SpriteSheet::GetAnimation (int32 animation_id, float scale) const
{
    SDL_assert(animation_id >= 0);
    SDL_assert(static_cast<size_t>(animation_id) < this->animations.size());

    auto result = this->animations[animation_id].value;
    for (auto& frame : result.frames)
    {
        frame.scale(scale);
    }

    return result;
}

} // namespace rdge
