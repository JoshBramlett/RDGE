#include <rdge/physics/collision.hpp>
#include <rdge/physics/shapes/polygon.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/util/logger.hpp>

#include <SDL_assert.h>

namespace rdge {
namespace physics {

using namespace rdge::math;

bool
intersects (const polygon& p, const circle& c, collision_manifold& mf)
{
    mf.count = 0;

    float sep_max = std::numeric_limits<float>::lowest();
    size_t index_a = 0;
    for (size_t i = 0; i < p.count; i++)
    {
        float sep_axis = math::dot(p.normals[i], c.pos - p.vertices[i]);
        if (sep_axis > c.radius)
        {
            return false;
        }

        if (sep_axis > sep_max)
        {
            sep_max = sep_axis;
            index_a = i;
        }
    }

    size_t index_b = ((index_a + 1) < p.count) ? (index_a + 1) : 0;
    const auto& vertex_a = p.vertices[index_a];
    const auto& vertex_b = p.vertices[index_b];

    if (sep_max <= 0.f)
    {
        // Case 1) Circle center is inside the polygon
        mf.count = 1;
        mf.contacts[0] = c.pos;
        mf.depths[0] = sep_max;
        mf.normal = p.normals[index_a];
        mf.plane = (vertex_a + vertex_b) * 0.5f;
    }
    else if (math::dot(c.pos - vertex_a, vertex_b - vertex_a) <= 0.f)
    {
        // Case 2a) Circle center outside of polygon - test if vertex_a is closest
        // to the circle centroid.
        //
        // Note: If AB and A0 point in opposite directions we can infer vertex_a
        // is closest to the circle centroid.

        const auto& a_zero = c.pos - vertex_a;
        if (a_zero.self_dot() > math::square(c.radius))
        {
            return false;
        }

        mf.count = 1;
        mf.contacts[0] = c.pos;
        mf.depths[0] = c.radius - a_zero.length();
        mf.normal = a_zero.normalize();
        mf.plane = vertex_a;
    }
    else if (math::dot(c.pos - vertex_b, vertex_a - vertex_b) <= 0.f)
    {
        // Case 2b) Circle center outside of polygon - test if vertex_b is closest
        // to the circle centroid.

        const auto& b_zero = c.pos - vertex_b;
        if (b_zero.self_dot() > math::square(c.radius))
        {
            return false;
        }

        mf.count = 1;
        mf.contacts[0] = c.pos;
        mf.depths[0] = c.radius - b_zero.length();
        mf.normal = b_zero.normalize();
        mf.plane = vertex_b;
    }
    else
    {
        // Case 3) Cannot infer which vertex is closest.  Use the plane AB as our
        // reference edge.

        const auto& face_center = (vertex_a + vertex_b) * 0.5f;
        float s = math::dot(c.pos - face_center, p.normals[index_a]);
        if (s > c.radius)
        {
            return false;
        }

        mf.count = 1;
        mf.contacts[0] = c.pos;
        mf.depths[0] = s;
        mf.normal = p.normals[index_a];
        mf.plane = face_center;
    }

    return true;
}

std::ostream& operator<< (std::ostream& os, const collision_manifold& mf)
{
    if (mf.count == 0)
    {
        return os << "[ manifold: count=0 ]\n";
    }

    os << "manifold: ["
       << "\n  count=" << mf.count
       << "\n  normal=" << mf.normal
       << "\n  plane=" << mf.plane
       << "\n  flip=" << std::boolalpha << mf.flip;

    for (size_t i = 0; i < mf.count; i++)
    {
        os << "\n  contacts[" << i << "]=" << mf.contacts[i]
           << " depths[" << i << "]=" << mf.depths[i];
    }

    return os << "\n]\n";
}

std::ostream& operator<< (std::ostream& os, const contact_impulse& impulse)
{
    if (impulse.count == 0)
    {
        return os << "[ impulse: count=0 ]\n";
    }

    os << "impulse: ["
       << "\n  count=" << impulse.count;

    for (size_t i = 0; i < impulse.count; i++)
    {
        os << "\n  normals[" << i << "]=" << impulse.normals[i]
           << " tangents[" << i << "]=" << impulse.tangents[i];
    }

    return os << "\n]\n";
}

} // namespace physics
} // namespace rdge
