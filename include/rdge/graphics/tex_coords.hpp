//! \headerfile <rdge/graphics/isprite.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 12/08/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

enum class TexCoordsFlip
{
    NONE = 0,
    HORIZONTAL,
    VERTICAL
};

enum class TexCoordsRotation
{
    NONE = 0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270
};

//! \struct tex_coords
//! \brief Texture coordinate (UV) data for a sprite
//! \details Default values are set to fill the entire texture.
struct tex_coords
{
    static const tex_coords DEFAULT;
    static const tex_coords EMPTY;

    math::vec2 top_left     = { 0.f, 1.f };
    math::vec2 bottom_left  = { 0.f, 0.f };
    math::vec2 bottom_right = { 1.f, 0.f };
    math::vec2 top_right    = { 1.f, 1.f };

    //!@{ Subscript operators to access an element by index
    math::vec2& operator[] (uint32 index) noexcept;
    const math::vec2& operator[] (uint32 index) const noexcept;
    //!@}

    //!@{ Query state
    bool is_default (void) const noexcept;
    bool is_empty (void) const noexcept;
    //!@}

    //!@{ Basic tex_coords transforms
    void flip (TexCoordsFlip) noexcept;
    void rotate (TexCoordsRotation) noexcept;
    //!@}
};

//! \brief tex_coords equality operator
inline bool operator== (const tex_coords& lhs, const tex_coords& rhs) noexcept
{
    return (lhs.top_left == rhs.top_left) &&
           (lhs.bottom_left == rhs.bottom_left) &&
           (lhs.bottom_right == rhs.bottom_right) &&
           (lhs.top_right == rhs.top_right);
}

//! \brief tex_coords inequality operator
inline bool operator!= (const tex_coords& lhs, const tex_coords& rhs) noexcept
{
    return !(lhs == rhs);
}

//! \brief tex_coords stream output operator
std::ostream& operator<< (std::ostream& os, const tex_coords& value);

} // namespace rdge
