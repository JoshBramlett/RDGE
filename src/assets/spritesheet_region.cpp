#include <rdge/assets/spritesheet_region.hpp>

namespace rdge {

spritesheet_region&
spritesheet_region::flip_horizontal (void) noexcept
{
    this->coords.flip_horizontal();
    this->origin.x = this->size.w - this->origin.x;

    return *this;
}

spritesheet_region&
spritesheet_region::flip_vertical (void) noexcept
{
    this->coords.flip_vertical();
    this->origin.y = this->size.h - this->origin.y;

    return *this;
}

spritesheet_region
spritesheet_region::flip_horizontal (void) const noexcept
{
    spritesheet_region result = *this;
    return result.flip_horizontal();
}

spritesheet_region
spritesheet_region::flip_vertical (void) const noexcept
{
    spritesheet_region result = *this;
    return result.flip_vertical();
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
