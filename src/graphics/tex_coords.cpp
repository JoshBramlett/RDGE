#include <rdge/graphics/tex_coords.hpp>

#include <SDL_assert.h>

namespace rdge {

const tex_coords tex_coords::DEFAULT = { { 0.f, 1.f },
                                         { 0.f, 0.f },
                                         { 1.f, 0.f },
                                         { 1.f, 1.f } };
const tex_coords tex_coords::EMPTY = { };

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

bool
tex_coords::is_default (void) const noexcept
{
    return *this == tex_coords::DEFAULT;
}

bool
tex_coords::is_empty (void) const noexcept
{
    return *this == tex_coords::EMPTY;
}

void
tex_coords::flip (TexCoordsFlip f) noexcept
{
    if (f == TexCoordsFlip::HORIZONTAL)
    {
        std::swap(this->top_left, this->top_right);
        std::swap(this->bottom_left, this->bottom_right);
    }
    else if (f == TexCoordsFlip::VERTICAL)
    {
        std::swap(this->top_left, this->bottom_left);
        std::swap(this->top_right, this->bottom_right);
    }
}

void
tex_coords::rotate (TexCoordsRotation r) noexcept
{
    switch (r)
    {
    case TexCoordsRotation::ROTATE_90:
        {
            auto temp = this->top_left;
            this->top_left     = this->top_right;
            this->top_right    = this->bottom_right;
            this->bottom_right = this->bottom_left;
            this->bottom_left  = temp;
        }

        break;
    case TexCoordsRotation::ROTATE_180:
        std::swap(this->top_left, this->bottom_right);
        std::swap(this->top_right, this->bottom_left);

        break;
    case TexCoordsRotation::ROTATE_270:
        {
            auto temp = this->top_left;
            this->top_left     = this->bottom_left;
            this->bottom_left  = this->bottom_right;
            this->bottom_right = this->top_right;
            this->top_right    = temp;
        }

        break;
    case TexCoordsRotation::NONE:
    default:
        break;
    }
}

std::ostream& operator<< (std::ostream& os, const tex_coords& value)
{
    os << "[ top_left=" << value.top_left
       << " top_right=" << value.top_right
       << " bottom_right=" << value.bottom_right
       << " bottom_left=" << value.bottom_right << " ]";

    return os;
}

} // namespace rdge
