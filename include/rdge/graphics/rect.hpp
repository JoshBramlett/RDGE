//! \headerfile <rdge/graphics/rect.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/22/2016

// TODO Detemplatize !!!

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

#include <rdge/core.hpp>
#include <rdge/graphics/point.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec4.hpp>

#include <SDL2/SDL_rect.h>

#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

template <typename T, typename = void>
struct rect_t;

//! \struct rect_t
//! \brief Base templated type representing a rectangle
template <typename T>
struct rect_t <T, std::enable_if_t<std::is_arithmetic<T>::value>>
{
    T x; //!< x-coordinate
    T y; //!< y-coordinate
    T w; //!< width
    T h; //!< height

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
    explicit constexpr rect_t (const math::vec4& vec)
        : x(vec.x), y(vec.y), w(vec.z), h(vec.w)
    { }

    //! \brief rect_t ctor
    //! \details Initialize rect from point and size
    //! \param [in] point Point structure
    //! \param [in] size Size structure
    explicit constexpr rect_t (const point& point, const math::uivec2& size)
        : x(point.x)
        , y(point.y)
        , w(static_cast<int32>(size.w))
        , h(static_cast<int32>(size.h))
    { }

    //! \brief rect_t ctor
    //! \details Initialize rect from two vec2s representing point and size
    //! \param [in] point vec2 structure representing the location
    //! \param [in] size vec2 structure representing the size
    explicit constexpr rect_t (const math::vec2& point, const math::vec2& size)
        : x(point.x)
        , y(point.y)
        , w(size.x)
        , h(size.y)
    { }

    //! \brief User-defined conversion to vec4
    //! \details Casts values to float during conversion.
    //! \returns vec4 containing the rectangle data
    explicit constexpr operator math::vec4 (void) const
    {
        return math::vec4(static_cast<float>(x),
                          static_cast<float>(y),
                          static_cast<float>(w),
                          static_cast<float>(h));
    }

    //! \brief User-defined conversion to native SDL_Rect
    //! \details Casts values to signed integers during conversion.
    //! \returns SDL_Rect containing the rectangle data
    //! \see http://wiki.libsdl.org/SDL_Rect
    explicit constexpr operator SDL_Rect (void) const
    {
        return SDL_Rect {
            static_cast<int32>(x),
            static_cast<int32>(y),
            static_cast<int32>(w),
            static_cast<int32>(h)
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
    //constexpr point_t<T> top_left (void) const
    //{
        //return point_t<T>(x, y);
    //}

    //! \brief Get the top right corner of the rectangle
    //! \returns point_t structure
    //constexpr point_t<T> top_right (void) const
    //{
        //return point_t<T>(right(), y);
    //}

    //! \brief Get the bottom left corner of the rectangle
    //! \returns point_t structure
    //constexpr point_t<T> bottom_left (void) const
    //{
        //return point_t<T>(x, bottom());
    //}

    //! \brief Get the bottom right corner of the rectangle
    //! \returns point_t structure
    //constexpr point_t<T> bottom_right (void) const
    //{
        //return point_t<T>(right(), bottom());
    //}

    //! \brief Get the calculated center of the rectangle
    //! \returns point_t structure
    //constexpr point_t<T> centroid (void) const
    //{
        //return point_t<T>(x + (w / 2), y + (h / 2));
    //}

    //! \brief Check if a point resides within the Rect
    //! \param [in] point Point structure
    //! \returns True if point is within the rect, false otherwise
    constexpr bool contains (const point& p) const
    {
        return p.x >= x && p.y >= y &&
               p.x <= right() && p.y <= bottom();
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
using rect = rect_t<int32>;
//! \typedef RectF Float rect_t structure
using RectF = rect_t<float>;

//using RectFail = rect_t<std::string>;



} // namespace rdge
