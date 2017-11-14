//! \headerfile <rdge/graphics/color.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/22/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec4.hpp>

#include <SDL.h>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \struct color
//! \brief Container for RGBA colors
//! \details RGBA values are on an integer scale of 0-255, with user
//!          defined conversions to other relevant types.
struct color : public SDL_Color
{
    static const color BLACK;   //!< RGBA #000000FF
    static const color WHITE;   //!< RGBA #FFFFFFFF
    static const color RED;     //!< RGBA #FF0000FF
    static const color GREEN;   //!< RGBA #00FF00FF
    static const color BLUE;    //!< RGBA #0000FFFF
    static const color YELLOW;  //!< RGBA #FFFF00FF
    static const color CYAN;    //!< RGBA #00FFFFFF
    static const color MAGENTA; //!< RGBA #FF00FFFF

    //! \brief color ctor
    //! \details Zero initialization
    constexpr color (void)
        : SDL_Color{0, 0, 0, 0}
    { }

    //! \brief color ctor
    //! \details Initialize from SDL_Color
    //! \param [in] sdl_color SDL_Color struct
    explicit constexpr color (const SDL_Color& sdl_color)
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
    //! \returns Vector of normalized values
    explicit constexpr operator math::vec4 (void) const noexcept
    {
        return math::vec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
    }

    //!@{
    //! \brief Convert color to hex string
    //! \details Hex strings are upper case and contain a preceding '#'.
    //! \returns Hex string
    std::string to_rgb (void) const noexcept;
    std::string to_rgba (void) const noexcept;
    std::string to_argb (void) const noexcept;
    //!@}

    //!@{
    //! \brief Convert hex string to color
    //! \details Hex string can contain a preceding '#' and is not case
    //!          sensitive (i.e. "FF00CC" and "#ff00cc" are both valid).
    //!          Alpha channel is optional for functions that accept an
    //!          explicit value and if omitted will default to 0xFF.
    //! \param [in] value Hex color string
    //! \returns color structure
    //! \throws std::runtime_error Unable to perform conversion
    static color from_rgb (const std::string& value);
    static color from_rgba (const std::string& value);
    static color from_argb (const std::string& value);
    //!@}
};

//! \brief color equality operator
constexpr bool operator== (const color& lhs, const color& rhs) noexcept
{
    return (lhs.r == rhs.r) && (lhs.g == rhs.g) && (lhs.b == rhs.b) && (lhs.a == rhs.a);
}

//! \brief color inequality operator
constexpr bool operator!= (const color& lhs, const color& rhs) noexcept
{
    return !(lhs == rhs);
}

//! \brief color stream output operator
//! \note Formatted as an RGBA hex string
//! \see color::to_rgba
std::ostream& operator<< (std::ostream&, const color&);

//! \brief color string conversion
//! \note Formatted as an RGBA hex string
//! \see color::to_rgba
std::string to_string (const color&);

} // namespace rdge
