//! \headerfile <rdge/graphics/rect.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 12/23/2015

/* TODO - Methods/functionality missing from libSDL2pp
 *
 *  1)  operator overloads
 *  2)  GetUnion & GetExtension
 *  3)  GetIntersection and IntersectLine
 *  4)  operator< - Unsure of the correctness of the implementation.  It
 *                  tests Y only if X is equal
 *  5)  hash function - Not sure if neccessary
 *
 *
 *  .NET Rect
 *  http://referencesource.microsoft.com/#System.Drawing/commonui/System/Drawing/Rectangle.cs,17559e21008f381d
 *  https://msdn.microsoft.com/en-us/library/system.drawing.rectangle(v=vs.110).aspx
 */

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/point.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec4.hpp>

#include <SDL2/SDL_rect.h>

#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \struct rect_t
//! \brief Base templated type representing a rectangle


// 1) Works, but all non-arithmetic types compile, but have no functionality
//template <typename T, typename Enable = void>
//class rect_t { };

//template <typename T>
//class rect_t<T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type>




// 2)  Doesn't work, not sure why
//template <typename T>
//using EnableRectPolicy = typename std::enable_if<std::is_arithmetic<T>::value>::type;

//template <typename T>
//class rect_t<T, EnableRectPolicy<T>>



// 3) Works, and all non-arithmetic types fail to compile
template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value>>
struct rect_t;

template <typename T>
struct rect_t<T>
{
    //! \var x X-coordinate
    T x;
    //! \var y Y-coordinate
    T y;
    //! \var w Width
    T w;
    //! \var h Height
    T h;

    //! \brief rect_t ctor
    //! \details Initialize rect to [0,0,0,0]
    constexpr rect_t (void)
        : x(0), y(0), w(0), h(0)
    { }

    //! \brief rect_t ctor
    //! \details Initialize rect from X, Y, W and H values
    //! \param [in] x x-coordinate
    //! \param [in] y y-coordinate
    //! \param [in] w Width
    //! \param [in] h Height
    constexpr rect_t (T x, T y, T w, T h)
        : x(x), y(y), w(w), h(h)
    { }

    //! \brief rect_t ctor
    //! \details Initialize rect from vec4
    //! \param [in] vec vec4 structure
    explicit constexpr rect_t (const RDGE::Math::vec4& vec)
        : x(vec.x)
        , y(vec.y)
        , w(vec.z)
        , h(vec.w)
    { }

    //! \brief rect_t ctor
    //! \details Initialize rect from point and size
    //! \param [in] point Point structure
    //! \param [in] size Size structure
    explicit constexpr rect_t (const Point& point, const Size& size)
        : x(point.x)
        , y(point.y)
        , w(static_cast<RDGE::Int32>(size.w))
        , h(static_cast<RDGE::Int32>(size.h))
    { }

    //! \brief rect_t ctor
    //! \details Initialize rect from two vec2s representing point and size
    //! \param [in] point vec2 structure representing the location
    //! \param [in] size vec2 structure representing the size
    explicit constexpr rect_t (const RDGE::Math::vec2& point, const RDGE::Math::vec2& size)
        : x(point.x)
        , y(point.y)
        , w(size.x)
        , h(size.y)
    { }

    //! \brief rect_t Copy ctor
    //! \details Default-copyable
    constexpr rect_t (const rect_t&) noexcept = default;

    //! \brief rect_t Move ctor
    //! \details Default-movable
    constexpr rect_t (rect_t&&) noexcept = default;

    //! \brief rect_t Copy Assignment Operator
    //! \details Default-copyable
    rect_t& operator= (const rect_t&) noexcept = default;

    //! \brief rect_t Move Assignment Operator
    //! \details Default-movable
    rect_t& operator= (rect_t&&) noexcept = default;

    //! \brief User-defined conversion to vec4
    //! \details Casts values to float during conversion.
    //! \returns vec4 containing the rectangle data
    explicit constexpr operator RDGE::Math::vec4 (void) const
    {
        return RDGE::Math::vec4(
                                static_cast<float>(x),
                                static_cast<float>(y),
                                static_cast<float>(w),
                                static_cast<float>(h)
                               );
    }

    //! \brief User-defined conversion to native SDL_Rect
    //! \details Casts values to signed integers during conversion.
    //! \returns SDL_Rect containing the rectangle data
    //! \see http://wiki.libsdl.org/SDL_Rect
    explicit constexpr operator SDL_Rect (void) const
    {
        return SDL_Rect {
                         static_cast<RDGE::Int32>(x),
                         static_cast<RDGE::Int32>(y),
                         static_cast<RDGE::Int32>(w),
                         static_cast<RDGE::Int32>(h)
                        };
    }

    //! \brief Size of the rectangle
    //! \returns Size structure representing width and height
    //constexpr typename std::enable_if<std::numeric_limits<T>::is_integer, bool>::type
    //RDGE::Graphics::Size Size (void) const
    //{
        //return RDGE::Graphics::Size(w, h);
    //}

    //! \brief Gets the y coordinate of the top edge
    //! \returns Top edge of the rectangle
    constexpr T top (void) const { return y; }

    //! \brief Gets the x coordinate of the left edge
    //! \returns Left edge of the rectangle
    constexpr T left (void) const { return x; }

    //! \brief Gets the y coordinate of the bottom edge
    //! \details Calculated by the sum of the y and h values
    //! \returns Bottom edge of the rectangle
    constexpr T bottom (void) const { return y + h; }

    //! \brief Gets the x coordinate of the right edge
    //! \details Calculated by the sum of the x and w values
    //! \returns Right edge of the rectangle
    constexpr T right (void) const { return x + w; }

    //! \brief Get the top left corner of the rectangle
    //! \returns point_t structure
    constexpr point_t<T> top_left (void) const
    {
        return point_t<T>(x, y);
    }

    //! \brief Get the top right corner of the rectangle
    //! \returns point_t structure
    constexpr point_t<T> top_right (void) const
    {
        return point_t<T>(right(), y);
    }

    //! \brief Get the bottom left corner of the rectangle
    //! \returns point_t structure
    constexpr point_t<T> bottom_left (void) const
    {
        return point_t<T>(x, bottom());
    }

    //! \brief Get the bottom right corner of the rectangle
    //! \returns point_t structure
    constexpr point_t<T> bottom_right (void) const
    {
        return point_t<T>(right(), bottom());
    }

    //! \brief Get the calculated center of the rectangle
    //! \returns point_t structure
    constexpr point_t<T> centroid (void) const
    {
        return point_t<T>(x + (w / 2), y + (h / 2));
    }

    //! \brief Check if a point resides within the Rect
    //! \param [in] point Point structure
    //! \returns True if point is within the rect, false otherwise
    constexpr bool contains (const point_t<T>& point) const
    {
        return point.x >= x && point.y >= y &&
               point.x <= right() && point.y <= bottom();
    }

    //! \brief Check if a point resides within the Rect
    //! \param [in] px x coordinate
    //! \param [in] py y coordinate
    //! \returns True if point is within the rect, false otherwise
    constexpr bool contains (T px, T py) const
    {
        return px >= x && py >= y &&
               px <= right() && py <= bottom();
    }

    //! \brief Check if an entire rectangular region resides within the Rect
    //! \param [in] rect Rect structure
    //! \returns True if rect is within the Rect, false otherwise
    constexpr bool contains (const rect_t<T>& rect) const
    {
        return rect.x >= x && rect.y >= y &&
               rect.right() <= right() && rect.bottom() <= bottom();
    }

    //! \brief Check if any part of a rectangle instersects with Rect
    //! \param [in] rect Rect structure
    //! \returns True if rect intersects with the Rect, false otherwise
    constexpr bool intersects_with (const rect_t<T>& rect) const
    {
        return (rect.x < right()) &&
               (x < rect.right()) &&
               (rect.y < bottom()) &&
               (y < rect.bottom());
    }

    //! \brief Check if rectangle is empty
    //! \returns True if width/height is lte zero, otherwise false
    constexpr bool is_empty (void) const
    {
        return (w <= 0) || (h <= 0);
    }

    //! \brief Construct a Rect from a center point, width and height
    //! \param [in] center Coordinates of the center of the rectangle
    //! \param [in] size Dimensions of the rectangle
    //! \returns Rectangle structure
    //static constexpr rect_t<T> from_center (const Point& center, const class Size& size)
    //{
        //return Rect(
                    //center.x - (size.w / 2),
                    //center.y - (size.h / 2),
                    //size.w,
                    //size.h
                   //);
    //}
};

//! \brief rect_t equality operator
//! \param [in] a First rect to compare
//! \param [in] b Second rect to compare
//! \returns True iff rects are identical
template <typename T>
constexpr bool operator== (const rect_t<T>& a, const rect_t<T>& b)
{
    return (a.x == b.x) &&
           (a.y == b.y) &&
           (a.w == b.w) &&
           (a.h == b.h);
}

//! \brief rect_t inequality operator
//! \param [in] a First rect to compare
//! \param [in] b Second rect to compare
//! \returns True iff rects are not identical
template <typename T>
constexpr bool operator!= (const rect_t<T>& a, const rect_t<T>& b)
{
    return (a.x != b.x) ||
           (a.y != b.y) ||
           (a.w != b.w) ||
           (a.h != b.h);
}

//! \brief rect_t stream output operator
//! \param [in] os Output stream
//! \param [in] rect rect_t to write to the stream
//! \returns Output stream
template <typename T>
inline std::ostream& operator<< (std::ostream& os, const rect_t<T>& rect)
{
    return os << "[ x:" << rect.x
              << ", y:" << rect.y
              << ", w:" << rect.w
              << ", h:" << rect.h << " ]";
}

//! \typedef Rect Signed integer rect_t structure
using Rect = rect_t<RDGE::Int32>;
//! \typedef RectF Float rect_t structure
using RectF = rect_t<float>;

//using RectFail = rect_t<std::string>;



} // namespace Graphics
} // namespace RDGE
