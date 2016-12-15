//! \headerfile <rdge/gfx/color.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/22/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec4.hpp>

#include <SDL.h>

#include <string>
#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace gfx {

//! \struct color
//! \brief Container for RGBA colors
//! \details RGBA values are on an integer scale of 0-255, with user
//!          defined conversions to other relevant types.
struct color final : public SDL_Color
{
public:
    //! \brief Pre-defined Black color
    //! \returns black color object
    static constexpr color BLACK (void) { return {0, 0, 0}; }

    //! \brief Pre-defined White color
    //! \returns White color object
    static constexpr color WHITE (void) { return {255, 255, 255}; }

    //! \brief Pre-defined Red color
    //! \returns Red color object
    static constexpr color RED (void) { return {255, 0, 0}; }

    //! \brief Pre-defined Green color
    //! \returns Green color object
    static constexpr color GREEN (void) { return {0, 255, 0}; }

    //! \brief Pre-defined Blue color
    //! \returns Blue color object
    static constexpr color BLUE (void) { return {0, 0, 255}; }

    //! \brief Pre-defined Yellow color
    //! \returns Yellow color object
    static constexpr color YELLOW (void) { return {255, 255, 0}; }

    //! \brief Pre-defined Cyan color
    //! \returns Cyan color object
    static constexpr color CYAN (void) { return {0, 255, 255}; }

    //! \brief Pre-defined Magenta color
    //! \returns Magenta color object
    static constexpr color MAGENTA (void) { return {255, 0, 255}; }

    //! \brief color ctor
    //! \details Initialize color to [0,0,0,0]
    constexpr color (void)
        : SDL_Color{0, 0, 0, 0}
    { }

    //! \brief color ctor
    //! \details Initialize color from RGBA values
    //! \param [in] r Red value
    //! \param [in] g Green value
    //! \param [in] b Blue value
    //! \param [in] a Alpha value (defaults to 255)
    constexpr color (rdge::uint8 r, rdge::uint8 g, rdge::uint8 b, rdge::uint8 a = 255)
        : SDL_Color{r, g, b, a}
    { }

    //! \brief color dtor
    ~color (void) noexcept = default;

    //! \brief color Copy ctor
    //! \details Default-copyable
    constexpr color (const color&) = default;

    //! \brief color Move ctor
    //! \details Default-movable
    constexpr color (color&&) = default;

    //! \brief color Copy assignment operator
    //! \details Default-copyable
    color& operator= (const color&) noexcept = default;

    //! \brief color Move assignment operator
    //! \details Default-movable
    color& operator= (color&&) noexcept = default;

    //! \brief User-defined conversion to UInt32
    //! \details Checks endianness to return appropriate format.  The
    //!          conversion is marked explicit so it must be used with
    //!          direct-initialization or explicit conversions.
    //! \returns Unsigned integer of the color data
    explicit constexpr operator rdge::uint32 (void) const noexcept
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
    explicit constexpr operator rdge::math::vec4 (void) const noexcept
    {
        return rdge::math::vec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
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
std::ostream& operator<< (std::ostream& os, const rdge::gfx::color& value);

} // namespace gfx

//! \brief color conversion to string
//! \param [in] value color to convert
//! \returns std::string representation
std::string to_string (const rdge::gfx::color& value);

} // namespace rdge
