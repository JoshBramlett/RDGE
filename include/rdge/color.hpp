//! \headerfile <rdge/color.hpp>
//! \author Josh Bramlett
//! \version 0.0.4
//! \date 05/14/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/math/vec4.hpp>

#include <SDL.h>

#include <string>
#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

//! \class Color
//! \brief Storage representing RGBA colors
//! \details RGBA values are on an integer scale of 0-255, with user
//!          defined conversions to other relevant types.
class Color final : public SDL_Color
{
public:
    //! \brief Pre-defined Black color
    //! \returns Black color object
    static constexpr Color Black (void) { return {0, 0, 0}; }

    //! \brief Pre-defined White color
    //! \returns White color object
    static constexpr Color White (void) { return {255, 255, 255}; }

    //! \brief Pre-defined Red color
    //! \returns Red color object
    static constexpr Color Red (void) { return {255, 0, 0}; }

    //! \brief Pre-defined Green color
    //! \returns Green color object
    static constexpr Color Green (void) { return {0, 255, 0}; }

    //! \brief Pre-defined Blue color
    //! \returns Blue color object
    static constexpr Color Blue (void) { return {0, 0, 255}; }

    //! \brief Pre-defined Yellow color
    //! \returns Yellow color object
    static constexpr Color Yellow (void) { return {255, 255, 0}; }

    //! \brief Pre-defined Cyan color
    //! \returns Cyan color object
    static constexpr Color Cyan (void) { return {0, 255, 255}; }

    //! \brief Pre-defined Magenta color
    //! \returns Magenta color object
    static constexpr Color Magenta (void) { return {255, 0, 255}; }

    //! \brief Color ctor
    //! \details Initialize Color to [0,0,0,0]
    constexpr Color (void) noexcept
        : SDL_Color{0, 0, 0, 0}
    { }

    //! \brief Color ctor
    //! \details Initialize Color from RGBA values
    //! \param [in] r Red value
    //! \param [in] g Green value
    //! \param [in] b Blue value
    //! \param [in] a Alpha value (defaults to 255)
    constexpr Color (UInt8 r, UInt8 g, UInt8 b, UInt8 a = 255) noexcept
        : SDL_Color{r, g, b, a}
    { }

    //! \brief Color Copy ctor
    //! \details Default-copyable
    constexpr Color (const Color&) noexcept = default;

    //! \brief Color Move ctor
    //! \details Default-movable
    constexpr Color (Color&&) noexcept = default;

    //! \brief Color Copy Assignment Operator
    //! \details Default-copyable
    Color& operator= (const Color&) noexcept = default;

    //! \brief Color Move Assignment Operator
    //! \details Default-movable
    Color& operator= (Color&&) noexcept = default;

    //! \brief User-defined conversion to UInt32
    //! \details Checks endianness to return appropriate format.  The
    //!          conversion is marked explicit so it must be used with
    //!          direct-initialization or explicit conversions.
    //! \returns Unsigned integer of the color data
    explicit constexpr operator RDGE::UInt32 (void) const noexcept
    {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return r << 24 | g << 16 | b << 8 | a;
#else
        return a << 24 | b << 16 | g << 8 | r;
#endif
    }

    //! \brief User-defined conversion to a vector of floats
    //! \details The values will be in the range [0.0f-1.0f].  The
    //!          conversion is marked explicit so it must be used with
    //!          direct-initialization or explicit conversions.
    //! \returns Vector of clamped floats
    explicit constexpr operator RDGE::Math::vec4 (void) const noexcept
    {
        return RDGE::Math::vec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
    }

    //! \brief Build color from a case insensitive RGB string
    //! \details A valid format is a six digit hex string (optionally seven
    //!          with a preceding '#')  e.g. "FF00CC" or "#ff00cc".
    //! \param [in] color Hex color string
    //! \returns Color structure
    //! \throws std::runtime_error Unable to parse string
    static Color FromRGB (const std::string& color);

    //! \brief Build color from a case insensitive RGBA string
    //! \details A valid format is an eight digit hex string (optionally nine
    //!          with a preceding '#')  e.g. "FF00CCAA" or "#ff00ccaa".
    //! \param [in] color Hex color string
    //! \returns Color structure
    //! \throws std::runtime_error Unable to parse string
    static Color FromRGBA (const std::string& color);
};

//! \brief Color equality operator
//! \param [in] a First color to compare
//! \param [in] b Second color to compare
//! \returns True iff colors are identical
constexpr bool operator== (const Color& a, const Color& b)
{
    return (a.r == b.r) && (a.g == b.g) && (a.b == b.b) && (a.a == b.a);
}

//! \brief Color inequality operator
//! \param [in] a First color to compare
//! \param [in] b Second color to compare
//! \returns True iff colors are not identical
constexpr bool operator!= (const Color& a, const Color& b)
{
    return (a.r != b.r) || (a.g != b.g) || (a.b != b.b) || (a.a != b.a);
}

//! \brief Color stream output operator
//! \param [in] os Output stream
//! \param [in] color Color to write to the stream
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, const Color& color);

//! \brief Color conversion to string
//! \param [in] color Color to convert
//! \returns std::string representation
std::string to_string (const Color& color);

} // namespace rdge
