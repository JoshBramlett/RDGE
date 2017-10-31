#include <rdge/assets/spritesheet_region.hpp>

namespace rdge {

void
spritesheet_region::flip (TexCoordsFlip f) noexcept
{
    coords.flip(f);

    if (f == TexCoordsFlip::HORIZONTAL)
    {
        origin.x = size.w - origin.x;
    }
    else if (f == TexCoordsFlip::VERTICAL)
    {
        origin.y = size.h - origin.y;
    }
}

void
spritesheet_region::rotate (TexCoordsRotation r) noexcept
{
    coords.rotate(r);

    switch (r)
    {
    case TexCoordsRotation::ROTATE_90:
        origin = math::vec2(origin.y, size.w - origin.x);
        size = math::vec2(size.h, size.w);

        break;
    case TexCoordsRotation::ROTATE_180:
        origin = math::vec2(size.w - origin.x, size.h - origin.y);

        break;
    case TexCoordsRotation::ROTATE_270:
        origin = math::vec2(size.h - origin.y, origin.x);
        size = math::vec2(size.h, size.w);

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
    origin *= scale;
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
