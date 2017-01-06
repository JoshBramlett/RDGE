//! \headerfile <rdge/graphics/color.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/22/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec4.hpp>

#include <SDL.h>

#include <string>
#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct color
//! \brief Container for RGBA colors
//! \details RGBA values are on an integer scale of 0-255, with user
//!          defined conversions to other relevant types.
struct color final : public SDL_Color
{
public:
    static const color BLACK;   //!< Pre-defined Black color
    static const color WHITE;   //!< Pre-defined White color
    static const color RED;     //!< Pre-defined Red color
    static const color GREEN;   //!< Pre-defined Green color
    static const color BLUE;    //!< Pre-defined Blue color
    static const color YELLOW;  //!< Pre-defined Yellow color
    static const color CYAN;    //!< Pre-defined Cyan color
    static const color MAGENTA; //!< Pre-defined Magenta color

    //! \brief color ctor
    //! \details Initialize color to [0,0,0,0]
    constexpr color (void)
        : SDL_Color{0, 0, 0, 0}
    { }

    //! \brief color ctor
    //! \details Initialize from SDL_Color
    constexpr color (const SDL_Color& sdl_color)
        : SDL_Color{sdl_color}
    { }

    //! \brief color ctor
    //! \details Initialize color from RGBA values
    //! \param [in] r Red value
    //! \param [in] g Green value
    //! \param [in] b Blue value
    //! \param [in] a Alpha value (defaults to 255)
    constexpr color (uint8 r, uint8 g, uint8 b, uint8 a = 255)
        : SDL_Color{r, g, b, a}
    { }

    //! \brief color dtor
    ~color (void) noexcept = default;

    //!@{
    //! \brief Copy and move enabled
    constexpr color (const color&) = default;
    color& operator= (const color&) noexcept = default;
    constexpr color (color&&) = default;
    color& operator= (color&&) noexcept = default;
    //!@}

    //! \brief User-defined conversion to uint32
    //! \details Endianness check performed to return appropriate format.
    //! \note The conversion is explicit so it must be used with direct
    //!       initialization or explicit conversions.
    //! \returns Unsigned integer of the color data
    explicit constexpr operator uint32 (void) const noexcept
    {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return r << 24 | g << 16 | b << 8 | a;
#else
        return a << 24 | b << 16 | g << 8 | r;
#endif
    }

    //! \brief User-defined conversion to a vector of floats
    //! \details The values will be normalized to [0.f, 1.f].
    //! \note The conversion is explicit so it must be used with direct
    //!       initialization or explicit conversions.
    //! \returns Vector of clamped floats
    explicit constexpr operator math::vec4 (void) const noexcept
    {
        return math::vec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
    }

    //! \brief Build color from a case insensitive RGB string
    //! \details A valid format is a six digit hex string (optionally seven
    //!          with a preceding '#')  e.g. "FF00CC" or "#ff00cc".
    //! \param [in] value Hex color string
    //! \returns color structure
    //! \throws std::runtime_error Unable to parse string
    static color FromRGB (const std::string& value);

    //! \brief Build color from a case insensitive RGBA string
    //! \details A valid format is an eight digit hex string (optionally nine
    //!          with a preceding '#')  e.g. "FF00CCAA" or "#ff00ccaa".
    //! \param [in] value Hex color string
    //! \returns color structure
    //! \throws std::runtime_error Unable to parse string
    static color FromRGBA (const std::string& value);
};

//! \brief color equality operator
//! \param [in] a First color to compare
//! \param [in] b Second color to compare
//! \returns True iff colors are identical
constexpr bool operator== (const color& a, const color& b) noexcept
{
    return (a.r == b.r) && (a.g == b.g) && (a.b == b.b) && (a.a == b.a);
}

//! \brief color inequality operator
//! \param [in] a First color to compare
//! \param [in] b Second color to compare
//! \returns True iff colors are not identical
constexpr bool operator!= (const color& a, const color& b) noexcept
{
    return (a.r != b.r) || (a.g != b.g) || (a.b != b.b) || (a.a != b.a);
}

//! \brief color stream output operator
//! \param [in] os Output stream
//! \param [in] value color to write to the stream
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, const color& value);

//! \brief color conversion to string
//! \param [in] value color to convert
//! \returns std::string representation
std::string to_string (const color& value);

} // namespace rdge
