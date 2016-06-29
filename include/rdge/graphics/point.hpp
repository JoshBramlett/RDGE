//! \headerfile <rdge/graphics/point.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/22/2015

/* TODO - Methods/functionality missing from libSDL2pp
 *
 *  1)  Clamp and GetClamped - Used for fitting a point inside a Rect
 *  2)  Wrap and GetWrapped - Not entirely sure what this does
 *  3)  operator< - Unsure of the correctness of the implementation.  It
 *                  tests Y only if X is equal
 *  4)  hash function - Not sure if neccessary
 */

#pragma once

#include <rdge/types.hpp>

#include <SDL2/SDL_rect.h>

#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class Point
//! \brief Structure of x and y coordinates that define a point in a
//!        two-dimensional "Cartesian" plane.
//! \details Derived from SDL_Point and provides helper methods for
//!          coordinate values
//! \see http://wiki.libsdl.org/SDL_Point
class Point final : public SDL_Point
{
public:
    //! \brief Empty
    //! \returns An empty Point object
    static constexpr Point Empty (void) { return {0, 0}; };

    //! \brief Point ctor
    //! \details Initialize point to [0,0]
    constexpr Point (void)
        : SDL_Point{0, 0}
    { }

    //! \brief Point ctor
    //! \details Initialize point from an SDL_Point
    //! \param [in] point SDL_Point structure
    constexpr Point (const SDL_Point& point)
        : SDL_Point{point.x, point.y}
    { }

    //! \brief Point ctor
    //! \details Initialize point from X and Y values
    //! \param [in] x X Coordinate
    //! \param [in] y Y Coordinate
    constexpr Point (RDGE::Int32 x, RDGE::Int32 y)
        : SDL_Point{x, y}
    { }

    //! \brief Point Copy ctor
    //! \details Default-copyable
    Point (const Point&) noexcept = default;

    //! \brief Point Move ctor
    //! \details Default-movable
    Point (Point&&) noexcept = default;

    //! \brief Point Copy Assignment Operator
    //! \details Default-copyable
    Point& operator=(const Point&) noexcept = default;

    //! \brief Point Move Assignment Operator
    //! \details Default-movable
    Point& operator=(Point&&) noexcept = default;

    //! \brief Point memberwise negation
    //! \returns Negated copy
    constexpr Point operator-() const
    {
        return Point(-x, -y);
    }

    //! \brief Point memberwise addition
    //! \param [in] rhs Point to add
    //! \returns Copy of lvalue after rvalue memberwise addition
    constexpr Point operator+(const Point& rhs) const
    {
        return Point(x + rhs.x, y + rhs.y);
    }

    //! \brief Point memberwise subtraction
    //! \param [in] rhs Point to subtract
    //! \returns Copy of lvalue after rvalue memberwise subtraction
    constexpr Point operator-(const Point& rhs) const
    {
        return Point(x - rhs.x, y - rhs.y);
    }

    //! \brief Point memberwise multiplication
    //! \param [in] rhs Point to multiply
    //! \returns Copy of lvalue after rvalue memberwise multiplication
    constexpr Point operator*(const Point& rhs) const
    {
        return Point(x * rhs.x, y * rhs.y);
    }

    //! \brief Point memberwise multiplication
    //! \param [in] value Multiplier of both x and y values
    //! \returns Copy of lvalue after rvalue memberwise multiplication
    constexpr Point operator*(RDGE::Int32 value) const
    {
        return Point(x * value, y * value);
    }

    //! \brief Point memberwise division
    //! \param [in] rhs Point representing the divisor
    //! \returns Copy of dividend lvalue after rvalue memberwise division
    constexpr Point operator/(const Point& rhs) const
    {
        return Point(x / rhs.x, y / rhs.y);
    }

    //! \brief Point memberwise division
    //! \param [in] value Divisor of both x and y values
    //! \returns Copy of dividend lvalue after rvalue memberwise division
    constexpr Point operator/(RDGE::Int32 value) const
    {
        return Point(x / value, y / value);
    }

    //! \brief Point memberwise modulo (remainder) operation
    //! \param [in] rhs Point representing the divisor
    //! \returns Copy of dividend lvalue containing the modulus after
    //!          rvalue memberwise division
    constexpr Point operator%(const Point& rhs) const
    {
        return Point(x % rhs.x, y % rhs.y);
    }

    //! \brief Point memberwise modulo (remainder) operation
    //! \param [in] value Divisor of both x and y values
    //! \returns Copy of dividend lvalue containing the modulus after
    //!          rvalue memberwise division
    constexpr Point operator%(RDGE::Int32 value) const
    {
        return Point(x % value, y % value);
    }

    //! \brief Point memberwise addition
    //! \param [in] rhs Point to add
    //! \returns Reference to self
    Point& operator+=(const Point& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    //! \brief Point memberwise subtraction
    //! \param [in] rhs Point to subtract
    //! \returns Reference to self
    Point& operator-=(const Point& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    //! \brief Point memberwise multiplication
    //! \param [in] rhs Point to multiply
    //! \returns Reference to self
    Point& operator*=(const Point& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    //! \brief Point memberwise multiplication
    //! \param [in] value Multiplier of both x and y values
    //! \returns Reference to self
    Point& operator*=(RDGE::UInt32 value)
    {
        x *= value;
        y *= value;
        return *this;
    }

    //! \brief Point memberwise division
    //! \param [in] rhs Point representing the divisor
    //! \returns Reference to self
    Point& operator/=(const Point& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    //! \brief Point memberwise division
    //! \param [in] value Divisor of both x and y values
    //! \returns Reference to self (dividend)
    Point& operator/=(RDGE::UInt32 value)
    {
        x /= value;
        y /= value;
        return *this;
    }

    //! \brief Point memberwise modulo (remainder) operation
    //! \param [in] rhs Point representing the divisor
    //! \returns Reference to self (dividend)
    Point& operator%=(const Point& rhs)
    {
        x %= rhs.x;
        y %= rhs.y;
        return *this;
    }

    //! \brief Point memberwise modulo (remainder) operation
    //! \param [in] value Divisor of both x and y values
    //! \returns Reference to self (dividend)
    Point& operator%=(RDGE::UInt32 value)
    {
        x %= value;
        y %= value;
        return *this;
    }

    //! \brief Check if structure is empty
    //! \returns True if x and y are zero, otherwise false
    bool IsEmpty (void) const
    {
        return (x == 0) && (y == 0);
    }
};

//! \brief Point equality operator
//! \param [in] a First point to compare
//! \param [in] b Second point to compare
//! \returns True iff points are identical
constexpr bool operator==(const Point& a, const Point& b)
{
    return (a.x == b.x) && (a.y == b.y);
}

//! \brief Point inequality operator
//! \param [in] a First point to compare
//! \param [in] b Second point to compare
//! \returns True iff points are not identical
constexpr bool operator!=(const Point& a, const Point& b)
{
    return (a.x != b.x) || (a.y != b.y);
}

//! \brief Point stream output operator
//! \param [in] os Output stream
//! \param [in] point Point to write to the stream
//! \returns Output stream
inline std::ostream& operator<< (std::ostream& os, const Point& point)
{
    return os << "[" << point.x << "," << point.y << "]";
}

} // namespace Graphics
} // namespace RDGE
