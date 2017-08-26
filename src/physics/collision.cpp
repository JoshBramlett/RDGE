#include <rdge/physics/collision.hpp>
#include <rdge/physics/shapes/polygon.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/util/logger.hpp>

#include <SDL_assert.h>

namespace rdge {
namespace physics {

using namespace rdge::math;

bool
intersects_with (const polygon& p, const circle& c, collision_manifold& mf)
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
    const auto& face_a = p.vertices[index_a];
    const auto& face_b = p.vertices[index_b];

    if (sep_max <= 0.f)
    {
        // Circle center is inside the polygon

        // plane is the max separating vertex, in the direction of it's normal.
        // the point of contact is the circle centroid projected onto that plane.
        half_plane hp = {
            p.normals[index_a],
            math::dot(p.normals[index_a], face_a)
        };

        mf.count = 1;
        mf.contacts[0] = project(hp, c.pos);
        mf.depths[0] = sep_max;
        mf.normal = p.normals[index_a];
        mf.local_plane = (face_a + face_b) * 0.5f;
    }
    else if (math::dot(c.pos - face_a, face_b - face_a) <= 0.f)
    {
        if (math::dot(c.pos, face_a) > math::square(c.radius))
        {
            return false;
        }

        // Circle center outside of polygon, and the edge AB and A0 (closest
        // point to circle centroid) point in the same direction
        const auto& normal = c.pos - face_a;
        mf.count = 1;
        mf.contacts[0] = c.pos;
        mf.depths[0] = c.radius - normal.length();
        mf.normal = normal.normalize();
        mf.local_plane = (face_a + face_b) * 0.5f;
    }
    else if (math::dot(c.pos - face_b, face_a - face_b) <= 0.f)
    {
        if (math::dot(c.pos, face_b) > math::square(c.radius))
        {
            return false;
        }

        // Circle center outside of polygon, and the edge AB and A0 (closest
        // point to circle centroid) point in the same direction
        const auto& normal = c.pos - face_b;
        mf.count = 1;
        mf.contacts[0] = c.pos;
        mf.depths[0] = c.radius - normal.length();
        mf.normal = normal.normalize();
        mf.local_plane = (face_a + face_b) * 0.5f;
    }
    else
    {
        const auto& face_center = (face_a + face_b) * 0.5f;
        float s = math::dot(c.pos - face_center, p.normals[index_a]);
        if (s > c.radius)
        {
            return false;
        }

        mf.count = 1;
        mf.contacts[0] = c.pos;
        mf.depths[0] = math::dot(c.pos - face_center, p.normals[index_a]);
        mf.normal = p.normals[index_a];
        mf.local_plane = face_center;
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
