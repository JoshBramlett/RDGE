//! \headerfile <rdge/graphics/rect.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 03/30/2017


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
 *
 *
 *  TODO
 *
 *    1)  Consider moving to physics.  Also consider renaming to aabb, and if so what
 *        would be done with the screen_rect?  Could be a separate class altogether
 *        b/c I don't see any reason for collision methods on integral types.
 *    2)  Finish writing tests
 *    3)  Create a debug wireframe renderer
 *        see http://stackoverflow.com/questions/11594247/opengl-lines-with-shaders
 *    4)  Using the wireframe renderer, a collision sample could be written
 *    5)  rename math/functions to math/intrinsics
 *
 * sites of import:
 *
 * http://www.iforce2d.net/b2dtut/collision-anatomy
 * https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331
 *
 *
 *
 *  TODO Currently contains() is inclusive to the edges, but intersects_with() is
 *       not.  They should be consistent, but I'll wait to decide which is preferred
 *       after some more extensive use.
 */

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/physics/collision.hpp>

#include <SDL2/SDL_rect.h>

#include <algorithm>
#include <ostream>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

template <typename T, typename = void>
struct rect_t;

//! \struct rect_t
//! \brief Signed arithmetic typed structure defining an AABB rectangle
//! \details Structure is represented by two opposite points of the rectangle.
//!          Although there is technically no origin, in order to limit the math
//!          upon construction the first point will contain the min(x,y) and the
//!          second will store the max(x,y).  Therefore it could be considered
//!          that the bottom left corner is the origin.
template <typename T>
struct rect_t <T, std::enable_if_t<std::is_arithmetic<T>::value && std::is_signed<T>::value>>
{
    //! \typedef value_type rect_t type
    using value_type = T;

    math::vec2_t<T> pmin; //!< Lower x and y coordinate position
    math::vec2_t<T> pmax; //!< Higher x and y coordinate position

    //! \brief rect_t ctor
    //! \details Initialize rect to [0,0], [0,0].
    constexpr rect_t (void)
        : pmin(0, 0), pmax(0, 0)
    { }

    //! \brief rect_t ctor
    //! \details Initialize rect from min/max coordinates
    //! \param [in] min Min coordinate
    //! \param [in] max Max coordinate
    constexpr rect_t (const math::vec2_t<T>& min, const math::vec2_t<T>& max)
        : pmin(std::min(min.x, max.x), std::min(min.y, max.y))
        , pmax(std::max(min.x, max.x), std::max(min.y, max.y))
    { }

    //! \brief rect_t ctor
    //! \details Initialize rect from coordinate, width, and height values.
    //! \param [in] origin Bottom left point
    //! \param [in] width Rectangle width
    //! \param [in] height Rectangle height
    constexpr rect_t (const math::vec2_t<T>& origin, T width, T height)
        : pmin(origin)
        , pmax(origin.x + width, origin.y + height)
    { }

    //! \brief rect_t ctor
    //! \details Initialize rect from an SDL_Rect, with the origin being at
    //!          the upper left corner.  Since construction is specialized
    //!          with respect to the origin point it's recommended to specify
    //!          the \ref screen_rect type.
    //! \param [in] sdl_rect SDL_Rect structure
    explicit constexpr rect_t (const SDL_Rect& sdl_rect)
        : pmin(sdl_rect.x, sdl_rect.y - sdl_rect.h)
        , pmax(sdl_rect.x + sdl_rect.w, sdl_rect.y)
    {
        static_assert(std::is_same<T, int32>::value,
                      "Construction with SDL_Rect requires an int32 type");
    }

    //! \brief User-defined conversion to native SDL_Rect
    //! \details SDL_Rect is defined with an upper left origin
    //! \returns SDL_Rect containing the rectangle data
    //! \see http://wiki.libsdl.org/SDL_Rect
    explicit constexpr operator SDL_Rect (void) const
    {
        static_assert(std::is_same<T, int32>::value,
                      "Conversion to an SDL_Rect requires an int32 type");

        return SDL_Rect { pmin.x, pmax.y, width(), height() };
    }

    //!@{ Size values
    constexpr T width (void) const noexcept { return math::abs(pmax.x - pmin.x); }
    constexpr T height (void) const noexcept { return math::abs(pmax.y - pmin.y); }
    //!@}

    //!@{ Edge values
    constexpr T top (void) const noexcept { return pmax.y; }
    constexpr T left (void) const noexcept { return pmin.x; }
    constexpr T bottom (void) const noexcept { return pmin.y; }
    constexpr T right (void) const noexcept { return pmax.x; }
    //!@}

    //!@{ Corner values
    constexpr math::vec2_t<T> top_left (void) const noexcept { return { left(), top() }; }
    constexpr math::vec2_t<T> top_right (void) const noexcept { return { right(), top() }; }
    constexpr math::vec2_t<T> bottom_left (void) const noexcept { return { left(), bottom() }; }
    constexpr math::vec2_t<T> bottom_right (void) const noexcept { return { right(), bottom() }; }
    //!@}

    //! \brief Get the calculated center of the rectangle
    //! \returns vec2 structure
    constexpr math::vec2 centroid (void) const noexcept
    {
        return (static_cast<math::vec2>(pmin + pmax)) * 0.5f;
    }

    //! \brief Get the edge distances to the centroid
    //! \returns vec2 structure
    constexpr math::vec2 half_extent (void) const noexcept
    {
        return (static_cast<math::vec2>(pmin - pmax)).abs() * 0.5f;
    }

    //! \brief Check if a point resides within the rect (inclusive)
    //! \param [in] px x coordinate
    //! \param [in] py y coordinate
    //! \returns True iff the point is within the rect
    constexpr bool contains (T px, T py) const noexcept
    {
        return px >= left() && px <= right() &&
               py >= bottom() && py <= top();
    }

    //! \brief Check if a point resides within the rect (inclusive)
    //! \param [in] point Point coordinates
    //! \returns True iff the point is within the rect
    constexpr bool contains (const math::vec2_t<T>& point) const noexcept
    {
        return contains(point.x, point.y);
    }

    //! \brief Check if an entire rectangular region resides within the Rect (inclusive)
    //! \param [in] other rect_t structure
    //! \returns True iff other is within the rect
    constexpr bool contains (const rect_t<T>& other) const noexcept
    {
        return other.left() >= left() && other.right() <= right() &&
               other.bottom() >= bottom() && other.top() <= top();
    }

    //! \brief Check if the AABB rect intersects with another
    //! \param [in] other rect_t structure
    //! \returns True if intersecting
    constexpr bool intersects_with (const rect_t<T>& other) const
    {
        return (other.left() < right()) &&
               (left() < other.right()) &&
               (other.top() < bottom()) &&
               (top() < other.bottom());
    }

    //! \brief Check if the AABB rect intersects with another
    //! \details The provided \ref collision_manifold will be populated with details
    //!          on how the collision could be resolved.  If there was no collision
    //!          the manifold count will be set to zero.
    //! \param [in] other rect_t structure
    //! \param [out] mf Manifold containing resolution
    //! \returns True if intersecting
    bool intersects_with (const rect_t<T>& other, collision_manifold& mf)
    {
        mf.count = 0;
        math::vec2 cen_a = centroid();
        math::vec2 ext_a = half_extent();
        math::vec2 cen_b = other.centroid();
        math::vec2 ext_b = other.half_extent();
        math::vec2 d = cen_b - cen_a;

        float overlap_x = ext_a.x + ext_b.x - math::abs(d.x);
        if (overlap_x < 0.f)
        {
            return false;
        }

        float overlap_y = ext_a.y + ext_b.y - math::abs(d.y);
        if (overlap_y < 0.f)
        {
            return false;
        }

        float sign_x = (d.x < 0.f) ? -1.f : 1.f;
        float sign_y = (d.y < 0.f) ? -1.f : 1.f;

        mf.count = 1;
        if (overlap_x < overlap_y)
        {
            mf.depths[0] = overlap_x;
            mf.normal = { 1.f * sign_x, 0.f };
            mf.contacts[0] = { cen_a.x + (ext_a.x * sign_x),
                               cen_b.y - (ext_b.y * sign_y) };
        }
        else
        {
            mf.depths[0] = overlap_y;
            mf.normal = { 0.f, 1.f * sign_y };
            mf.contacts[0] = { cen_b.x - (ext_b.x * sign_x),
                               cen_a.y + (ext_a.y * sign_y) };
        }

        return true;
    }
};

//! \brief rect_t equality operator
//! \param [in] lhs First rect to compare
//! \param [in] rhs Second rect to compare
//! \returns True iff rects are identical
template <typename T>
constexpr bool operator== (const rect_t<T>& lhs, const rect_t<T>& rhs)
{
    return (lhs.x == rhs.x) &&
           (lhs.y == rhs.y) &&
           (lhs.w == rhs.w) &&
           (lhs.h == rhs.h);
}

//! \brief rect_t inequality operator
//! \param [in] lhs First rect to compare
//! \param [in] rhs Second rect to compare
//! \returns True iff rects are not identical
template <typename T>
constexpr bool operator!= (const rect_t<T>& lhs, const rect_t<T>& rhs)
{
    return !(lhs == rhs);
}

//! \brief rect_t stream output operator
//! \param [in] os Output stream
//! \param [in] rect rect_t to write to the stream
//! \returns Output stream
template <typename T>
inline std::ostream& operator<< (std::ostream& os, const rect_t<T>& rect)
{
    return os << "[ " << rect.pmin << ", " << rect.pmax << " ]";
}

using rect        = rect_t<float>; //!< Default floating point type
using screen_rect = rect_t<int32>; //!< Signed type (SDL_Rect compatible type)

} // namespace rdge
