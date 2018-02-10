#include <rdge/assets/spritesheet_region.hpp>

namespace rdge {

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
    sprite_offset *= sprite_offset;
    sprite_size *= sprite_size;
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
