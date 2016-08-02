//! \headerfile <rdge/graphics/point.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/22/2015

#pragma once

#include <rdge/types.hpp>
#include <rdge/math/vec2.hpp>

#include <SDL2/SDL_rect.h>

#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \struct point_t
//! \brief Base templated type representing a point on a cartesian plane
template <typename T>
struct point_t
{
    //! \union Data represents x and y coordinates, or width and height (used when
    //!        Size (unsigned) typedef)
    union
    {
        struct { T x, y; };
        struct { T w, h; };
    };

    //! \brief point_t ctor
    //! \details Initialize point to [0,0]
    constexpr point_t (void)
        : x(0), y(0)
    { }

    //! \brief point_t ctor
    //! \details Initialize point from X and Y values
    //! \param [in] x X Coordinate
    //! \param [in] y Y Coordinate
    constexpr point_t (T x, T y)
        : x(x), y(y)
    { }

    //! \brief point_t ctor
    //! \details Initialize point from a vec2
    //! \param [in] vec vec2 structure
    explicit constexpr point_t (const RDGE::Math::vec2& vec)
        : x(vec.x), y(vec.y)
    { }

    //! \brief point_t ctor
    //! \details Initialize point from an SDL_Point
    //! \param [in] point SDL_Point structure
    explicit constexpr point_t (const SDL_Point& point)
        : x(point.x), y(point.y)
    { }

    //! \brief point_t Copy ctor
    //! \details Default-copyable
    constexpr point_t (const point_t&) noexcept = default;

    //! \brief point_t Move ctor
    //! \details Default-movable
    constexpr point_t (point_t&&) noexcept = default;

    //! \brief point_t Copy Assignment Operator
    //! \details Default-copyable
    point_t& operator= (const point_t&) noexcept = default;

    //! \brief point_t Move Assignment Operator
    //! \details Default-movable
    point_t& operator= (point_t&&) noexcept = default;

    //! \brief User-defined conversion to vec2
    //! \details Casts values to float during conversion.
    //! \returns vec2 containing the point data
    explicit constexpr operator RDGE::Math::vec2 (void) const
    {
        return RDGE::Math::vec2(static_cast<float>(x), static_cast<float>(y));
    }

    //! \brief User-defined conversion to native SDL_Point
    //! \details Casts values to signed integers during conversion.
    //! \returns SDL_Point containing the point data
    //! \see http://wiki.libsdl.org/SDL_Point
    explicit constexpr operator SDL_Point (void) const
    {
        return SDL_Point { static_cast<RDGE::Int32>(x), static_cast<RDGE::Int32>(y) };
    }

    //! \brief point_t memberwise addition
    //! \param [in] rhs point_t to add
    //! \returns Reference to self
    point_t<T>& operator+= (const point_t<T>& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    //! \brief point_t memberwise subtraction
    //! \param [in] rhs point_t to subtract
    //! \returns Reference to self
    point_t<T>& operator-= (const point_t<T>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    //! \brief point_t memberwise multiplication
    //! \param [in] rhs point_t to multiply
    //! \returns Reference to self
    point_t<T>& operator*= (const point_t<T>& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    //! \brief point_t memberwise scalar multiplication
    //! \param [in] scalar Scalar to multiply
    //! \returns Reference to self
    point_t<T>& operator*= (T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    //! \brief point_t memberwise division
    //! \param [in] rhs point_t representing the divisor
    //! \returns Reference to self (quotient)
    point_t<T>& operator/= (const point_t<T>& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    //! \brief point_t memberwise division
    //! \param [in] scalar Divisor of both x and y values
    //! \returns Reference to self (quotient)
    point_t<T>& operator/= (T scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    //! \brief point_t memberwise modulo (remainder) operation
    //! \param [in] rhs point_t representing the divisor
    //! \returns Reference to self (remainder)
    point_t<T>& operator%= (const point_t<T>& rhs)
    {
        x %= rhs.x;
        y %= rhs.y;
        return *this;
    }

    //! \brief point_t memberwise modulo (remainder) operation
    //! \param [in] scalar Divisor of both x and y values
    //! \returns Reference to self (remainder)
    point_t<T>& operator%= (T scalar)
    {
        x %= scalar;
        y %= scalar;
        return *this;
    }
};

//! \brief point_t equality operator
//! \param [in] lhs Left side point_t to compare
//! \param [in] rhs Right side point_t to compare
//! \returns True iff points are identical
template <typename T>
constexpr bool operator== (const point_t<T>& lhs, const point_t<T>& rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

//! \brief point_t inequality operator
//! \param [in] lhs Left side point_t to compare
//! \param [in] rhs Right side point_t to compare
//! \returns True iff points are not identical
template <typename T>
constexpr bool operator!= (const point_t<T>& lhs, const point_t<T>& rhs)
{
    return !(lhs == rhs);
}

//! \brief point_t negation operator
//! \returns Negated point_t
template <typename T>
constexpr const point_t<T> operator- (const point_t<T>& value)
{
    return point_t<T>(-value.x, -value.y);
}

//! \brief point_t addition operator
//! \param [in] lhs Left side point_t to add
//! \param [in] rhs Right side point_t to add
//! \returns point_t of added values
template <typename T>
constexpr const point_t<T> operator+ (const point_t<T>& lhs, const point_t<T>& rhs)
{
    return point_t<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}

//! \brief point_t subtraction operator
//! \param [in] lhs Left side point_t to subtract from
//! \param [in] rhs Right side point_t
//! \returns point_t of subtracted values
template <typename T>
constexpr const point_t<T> operator- (const point_t<T>& lhs, const point_t<T>& rhs)
{
    return point_t<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

//! \brief point_t multiplication operator
//! \param [in] lhs Left side point_t to multiply
//! \param [in] rhs Right side point_t to multiply
//! \returns point_t of multiplied values
template <typename T>
constexpr const point_t<T> operator* (const point_t<T>& lhs, const point_t<T>& rhs)
{
    return point_t<T>(lhs.x * rhs.x, lhs.y * rhs.y);
}

//! \brief point_t scalar multiplication operator
//! \param [in] point point_t to multiply
//! \param [in] scalar multiplier
//! \returns point_t of multiplied values
template <typename T>
constexpr const point_t<T> operator* (const point_t<T>& point, T scalar)
{
    return point_t<T>(point.x * scalar, point.y * scalar);
}

//! \brief point_t division operator
//! \param [in] lhs Left side point_t numerator
//! \param [in] rhs Right side point_t denominator
//! \returns point_t quotient
template <typename T>
constexpr const point_t<T> operator/ (const point_t<T>& lhs, const point_t<T>& rhs)
{
    return point_t<T>(lhs.x / rhs.x, lhs.y / rhs.y);
}

//! \brief point_t scalar division operator
//! \param [in] point point_t numerator
//! \param [in] scalar denominator
//! \returns point_t quotient
template <typename T>
constexpr const point_t<T> operator/ (const point_t<T>& point, T scalar)
{
    return point_t<T>(point.x / scalar, point.y / scalar);
}

//! \brief point_t modulo operator
//! \param [in] lhs Left side point_t dividend
//! \param [in] rhs Right side point_t divisor
//! \returns point_t of remainder values
template <typename T>
constexpr const point_t<T> operator% (const point_t<T>& lhs, const point_t<T>& rhs)
{
    return point_t<T>(lhs.x % rhs.x, lhs.y % rhs.y);
}

//! \brief point_t scalar modulo operator
//! \param [in] point point_t dividend
//! \param [in] scalar divisor
//! \returns point_t of remainder values
template <typename T>
constexpr const point_t<T> operator% (const point_t<T>& point, T scalar)
{
    return point_t<T>(point.x % scalar, point.y % scalar);
}

//! \brief point_t stream output operator
//! \param [in] os Output stream
//! \param [in] point point_t to write to the stream
//! \returns Output stream
template <typename T>
inline std::ostream& operator<< (std::ostream& os, const point_t<T>& point)
{
    return os << "[" << point.x << "," << point.y << "]";
}

//! \typedef Point Signed integer point_t structure
using Point = point_t<RDGE::Int32>;
//! \typedef PointF Float point_t structure
using PointF = point_t<float>;
//! \typedef Size UInt32 point_t structure
using Size = point_t<RDGE::UInt32>;

} // namespace Graphics
} // namespace RDGE
