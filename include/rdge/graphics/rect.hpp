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
#include <rdge/graphics/size.hpp>

#include <SDL2/SDL_rect.h>

#include <ostream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Graphics {

//! \class Rect
//! \brief Represents the location and size of a rectangle
//! \details Derived from SDL_Rect and provides helper methods
//!          for rectangle manipulation
//! \see http://wiki.libsdl.org/SDL_Rect
class Rect final : public SDL_Rect
{
public:
    //! \brief Empty
    //! \returns An empty Rect object
    static constexpr Rect Empty (void) { return {0, 0, 0, 0}; };

    //! \brief Rect ctor
    //! \details Initialize rect to [0,0,0,0]
    constexpr Rect (void)
        : SDL_Rect{0, 0, 0, 0}
    { }

    //! \brief Rect ctor
    //! \details Initialize rect from X, Y, W and H values
    //! \param [in] x X-Coordinate
    //! \param [in] y Y-Coordinate
    //! \param [in] w Width
    //! \param [in] h Height
    constexpr Rect (
                    RDGE::Int32 x,
                    RDGE::Int32 y,
                    RDGE::Int32 w,
                    RDGE::Int32 h
                   )
        : SDL_Rect{x, y, w, h}
    { }

    //! \brief Rect ctor
    //! \details Initialize rect from point and size
    //! \param [in] point Point structure
    //! \param [in] size Size structure
    constexpr Rect (const Point& point, const Size& size)
        : SDL_Rect{
                   point.x,
                   point.y,
                   static_cast<RDGE::Int32>(size.w),
                   static_cast<RDGE::Int32>(size.h)
                  }
    { }

    //! \brief Rect Copy ctor
    //! \details Default-copyable
    constexpr Rect (const Rect&) noexcept = default;

    //! \brief Rect Move ctor
    //! \details Default-movable
    constexpr Rect (Rect&&) noexcept = default;

    //! \brief Rect Copy Assignment Operator
    //! \details Default-copyable
    Rect& operator=(const Rect&) noexcept = default;

    //! \brief Rect Move Assignment Operator
    //! \details Default-movable
    Rect& operator=(Rect&&) noexcept = default;

    //! \brief Size of the rectangle
    //! \returns Size structure representing width and height
    constexpr RDGE::Graphics::Size Size (void) const
    {
        return RDGE::Graphics::Size(w, h);
    }

    //! \brief Gets the y coordinate of the top edge
    //! \returns Int32 representing the top edge y coordinate
    constexpr RDGE::Int32 Top (void) const { return y; }

    //! \brief Gets the x coordinate of the left edge
    //! \returns Int32 representing the left edge x coordinate
    constexpr RDGE::Int32 Left (void) const { return x; }

    //! \brief Gets the y coordinate of the bottom edge
    //! \details Calculated by the sum of the y and h values
    //! \returns Int32 representing the bottom edge y coordinate
    constexpr RDGE::Int32 Bottom (void) const { return y + h; }

    //! \brief Gets the x coordinate of the right edge
    //! \details Calculated by the sum of the x and w values
    //! \returns Int32 representing the right edge x coordinate
    constexpr RDGE::Int32 Right (void) const { return x + w; }

    //! \brief Get the top left corner of the rectangle
    //! \returns Point structure
    constexpr Point TopLeft (void) const
    {
        return Point(x, y);
    }

    //! \brief Get the top right corner of the rectangle
    //! \returns Point structure
    constexpr Point TopRight (void) const
    {
        return Point(Right(), y);
    }

    //! \brief Get the bottom left corner of the rectangle
    //! \returns Point structure
    constexpr Point BottomLeft (void) const
    {
        return Point(x, Bottom());
    }

    //! \brief Get the bottom right corner of the rectangle
    //! \returns Point structure
    constexpr Point BottomRight (void) const
    {
        return Point(Right(), Bottom());
    }

    //! \brief Get the calculated center of the rectangle
    //! \returns Point structure
    constexpr Point Centroid (void) const
    {
        return Point(x + (w / 2), y + (h / 2));
    }

    //! \brief Check if a point resides within the Rect
    //! \param [in] point Point structure
    //! \returns True if point is within the rect, false otherwise
    constexpr bool Contains (const Point& point) const
    {
        return point.x >= x && point.y >= y &&
               point.x <= Right() && point.y <= Bottom();
    }

    //! \brief Check if a point resides within the Rect
    //! \param [in] px x coordinate
    //! \param [in] py y coordinate
    //! \returns True if point is within the rect, false otherwise
    constexpr bool Contains (RDGE::Int32 px, RDGE::Int32 py) const
    {
        return px >= x && py >= y &&
               px <= Right() && py <= Bottom();
    }

    //! \brief Check if an entire rectangular region resides within the Rect
    //! \param [in] rect Rect structure
    //! \returns True if rect is within the Rect, false otherwise
    constexpr bool Contains (const Rect& rect) const
    {
        return rect.x >= x && rect.y >= y &&
               rect.Right() <= Right() && rect.Bottom() <= Bottom();
    }

    //! \brief Check if any part of a rectangle instersects with Rect
    //! \param [in] rect Rect structure
    //! \returns True if rect intersects with the Rect, false otherwise
    constexpr bool IntersectsWith (const Rect& rect) const
    {
        return (rect.x < Right()) &&
               (x < rect.Right()) &&
               (rect.y < Bottom()) &&
               (y < rect.Bottom());
    }

    //! \brief Check if structure is empty
    //! \returns True if all values are zero, otherwise false
    bool IsEmpty (void) const
    {
        return (x == 0) && (y == 0) && (w == 0) && (h == 0);
    }

    //! \brief Construct a Rect from a center point, width and height
    //! \param [in] center Coordinates of the center of the rectangle
    //! \param [in] size Dimensions of the rectangle
    //! \returns Rectangle structure
    static constexpr Rect FromCenter (const Point& center, const class Size& size)
    {
        return Rect(
                    center.x - (size.w / 2),
                    center.y - (size.h / 2),
                    size.w,
                    size.h
                   );
    }
};

//! \brief Rect equality operator
//! \param [in] a First rect to compare
//! \param [in] b Second rect to compare
//! \returns True iff rects are identical
constexpr bool operator==(const Rect& a, const Rect& b)
{
    return (a.x == b.x) &&
           (a.y == b.y) &&
           (a.w == b.w) &&
           (a.h == b.h);
}

//! \brief Rect inequality operator
//! \param [in] a First rect to compare
//! \param [in] b Second rect to compare
//! \returns True iff rects are not identical
constexpr bool operator!=(const Rect& a, const Rect& b)
{
    return (a.x != b.x) ||
           (a.y != b.y) ||
           (a.w != b.w) ||
           (a.h != b.h);
}

//! \brief Rect stream output operator
//! \param [in] os Output stream
//! \param [in] rect Rect to write to the stream
//! \returns Output stream
inline std::ostream& operator<< (std::ostream& os, const Rect& rect)
{
    return os << "top left: [" << rect.x << "," << rect.y << "]"
              << " width: " << rect.w
              << " height: " << rect.h;
}

} // namespace Graphics
} // namespace RDGE
