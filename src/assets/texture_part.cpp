#include <rdge/assets/texture_part.hpp>

namespace rdge {

texture_part&
texture_part::flip_horizontal (void) noexcept
{
    this->coords.flip_horizontal();
    this->origin.x = this->size.w - this->origin.x;

    return *this;
}

texture_part&
texture_part::flip_vertical (void) noexcept
{
    this->coords.flip_vertical();
    this->origin.y = this->size.h - this->origin.y;

    return *this;
}

texture_part
texture_part::flip_horizontal (void) const noexcept
{
    texture_part result = *this;
    return result.flip_horizontal();
}

texture_part
texture_part::flip_vertical (void) const noexcept
{
    texture_part result = *this;
    return result.flip_vertical();
}

std::ostream& operator<< (std::ostream& os, const texture_part& p)
{
    os << "texture_part: ["
       << "\n  name=" << p.name
       << "\n  clip=" << p.clip
       << "\n  coords=" << p.coords
       << "\n  size=" << p.size
       << "\n  origin=" << p.origin
       << "\n]\n";

    return os;
}

} // namespace rdge
