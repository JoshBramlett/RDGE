#include <rdge/graphics/isprite.hpp>

#include <SDL_assert.h>

#include <sstream>

namespace rdge {

math::vec2&
tex_coords::operator[] (uint32 index) noexcept
{
    SDL_assert(index < 4);
    return (&this->top_left)[index];
}

const math::vec2&
tex_coords::operator[] (uint32 index) const noexcept
{
    SDL_assert(index < 4);
    return (&this->top_left)[index];
}

tex_coords&
tex_coords::flip_horizontal (void) noexcept
{
    std::swap(this->top_left, this->top_right);
    std::swap(this->bottom_left, this->bottom_right);

    return *this;
}

tex_coords&
tex_coords::flip_vertical (void) noexcept
{
    std::swap(this->top_left, this->bottom_left);
    std::swap(this->top_right, this->bottom_right);

    return *this;
}

tex_coords&
tex_coords::rotate_left (void) noexcept
{
    math::vec2 temp = this->top_left;
    this->top_left     = this->top_right;
    this->top_right    = this->bottom_right;
    this->bottom_right = this->bottom_left;
    this->bottom_left  = temp;

    return *this;
}

tex_coords&
tex_coords::rotate_right (void) noexcept
{
    math::vec2 temp = this->top_left;
    this->top_left     = this->bottom_left;
    this->bottom_left  = this->bottom_right;
    this->bottom_right = this->top_right;
    this->top_right    = temp;

    return *this;
}

tex_coords
tex_coords::flip_horizontal (void) const noexcept
{
    tex_coords result = *this;
    return result.flip_horizontal();
}

tex_coords
tex_coords::flip_vertical (void) const noexcept
{
    tex_coords result = *this;
    return result.flip_vertical();
}

tex_coords
tex_coords::rotate_left (void) const noexcept
{
    tex_coords result = *this;
    return result.rotate_left();
}

tex_coords
tex_coords::rotate_right (void) const noexcept
{
    tex_coords result = *this;
    return result.rotate_right();
}

std::ostream& operator<< (std::ostream& os, const tex_coords& value)
{
    std::ostringstream ss;
    ss << "[ top_left=" << value.top_left
       << " top_right=" << value.top_right
       << " bottom_right=" << value.bottom_right
       << " bottom_left=" << value.bottom_right << " ]";

    return os << ss.str();
}

} // namespace rdge
