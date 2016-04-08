//! \headerfile <rdge/color.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/29/2015
//! \bug

#pragma once

#include <rdge/types.hpp>
//#include <rdge/math/vec4.hpp>

#include <SDL.h>

#include <string>
#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

class Color final : public SDL_Color
{
public:
    //! \brief Pre-defined Black color
    //! \returns Black color object
    static constexpr Color Black (void) { return {0, 0, 0, 255}; }

    //! \brief Pre-defined White color
    //! \returns White color object
    static constexpr Color White (void) { return {255, 255, 255, 255}; }

    //! \brief Pre-defined Red color
    //! \returns Red color object
    static constexpr Color Red (void) { return {255, 0, 0, 255}; }

    //! \brief Pre-defined Green color
    //! \returns Green color object
    static constexpr Color Green (void) { return {0, 255, 0, 255}; }

    //! \brief Pre-defined Blue color
    //! \returns Blue color object
    static constexpr Color Blue (void) { return {0, 0, 255, 255}; }

    //! \brief Pre-defined Yellow color
    //! \returns Yellow color object
    static constexpr Color Yellow (void) { return {255, 255, 0, 255}; }

    //! \brief Pre-defined Cyan color
    //! \returns Cyan color object
    static constexpr Color Cyan (void) { return {0, 255, 255, 255}; }

    //! \brief Pre-defined Magenta color
    //! \returns Magenta color object
    static constexpr Color Magenta (void) { return {255, 0, 255, 255}; }

    constexpr Color (void)
        : SDL_Color{0, 0, 0, 0}
    { }

    constexpr Color (const SDL_Color& color)
        : SDL_Color{color.r, color.g, color.b, color.a}
    { }

    constexpr Color (RDGE::UInt8 r, RDGE::UInt8 g, RDGE::UInt8 b, RDGE::UInt8 a)
        : SDL_Color{r, g, b, a}
    { }

    //! \brief Color Copy ctor
    //! \details Default-copyable
    Color (const Color&) noexcept = default;

    //! \brief Color Move ctor
    //! \details Default-movable
    Color (Color&&) noexcept = default;

    //! \brief Color Copy Assignment Operator
    //! \details Default-copyable
    Color& operator=(const Color&) noexcept = default;

    //! \brief Color Move Assignment Operator
    //! \details Default-movable
    Color& operator=(Color&&) noexcept = default;

    constexpr RDGE::UInt32 ToRgba (void)
    {
        return a << 24 | b << 16 | g << 8 | r;
    }

    //constexpr RDGE::Math::vec4 ToVec4 (void)
    //{
        //return RDGE::Math::vec4(
                                //static_cast<float>(r / 255.0f),
                                //static_cast<float>(b / 255.0f),
                                //static_cast<float>(g / 255.0f),
                                //static_cast<float>(a / 255.0f)
                               //);
    //}

    //! \brief Build color from RGB string
    //! \param [in] color Hex color string
    //! \returns Color structure
    //! \throws Unable to parse string
    static Color FromRGB (const std::string& color);

    //! \brief Build color from RGBA string
    //! \param [in] color Hex color string
    //! \returns Color structure
    //! \throws Unable to parse string
    static Color FromRGBA (const std::string& color);
};

//! \brief Color equality operator
//! \param [in] a First color to compare
//! \param [in] b Second color to compare
//! \returns True iff colors are identical
constexpr bool operator==(const Color& a, const Color& b)
{
    return (a.r == b.r) && (a.g == b.g) && (a.b == b.b) && (a.a == b.a);
}

//! \brief Color inequality operator
//! \param [in] a First color to compare
//! \param [in] b Second color to compare
//! \returns True iff colors are not identical
constexpr bool operator!=(const Color& a, const Color& b)
{
    return (a.r != b.r) || (a.g != b.g) || (a.b != b.b) || (a.a != b.a);
}

//! \brief Color stream output operator
//! \param [in] os Output stream
//! \param [in] point Color to write to the stream
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, const Color& color);

} // namespace RDGE
