#include <rdge/physics/aabb.hpp>

namespace rdge {
namespace physics {

using namespace rdge::math;

bool
aabb::intersects_with (const aabb& other, collision_manifold& mf) const noexcept
{
    mf.count = 0;
    vec2 cen_a = centroid();
    vec2 ext_a = half_extent();
    vec2 cen_b = other.centroid();
    vec2 ext_b = other.half_extent();
    vec2 d = cen_b - cen_a;

    float overlap_x = ext_a.x + ext_b.x - std::fabs(d.x);
    if (overlap_x <= 0.f)
    {
        return false;
    }

    float overlap_y = ext_a.y + ext_b.y - std::fabs(d.y);
    if (overlap_y <= 0.f)
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

        // NOTE A distance of zero on the opposite axis means there are either two
        //      or four contact points (e.g. "T" or "+" shape, respectively) all
        //      on the same axis.  Currently only a single contact point is
        //      included in the manifold.

        if (d.y != 0.f || bottom() < other.bottom())
        {
            mf.contacts[0] = { cen_a.x + (ext_a.x * sign_x),
                               cen_b.y - (ext_b.y * sign_y) };
        }
        else
        {
            mf.contacts[0] = { cen_b.x + (ext_b.x * -sign_x),
                               cen_a.y - (ext_a.y * sign_y) };
        }
    }
    else
    {
        mf.depths[0] = overlap_y;
        mf.normal = { 0.f, 1.f * sign_y };

        if (d.x != 0.f || left() < other.left())
        {
            mf.contacts[0] = { cen_b.x - (ext_b.x * sign_x),
                               cen_a.y + (ext_a.y * sign_y) };
        }
        else
        {
            mf.contacts[0] = { cen_a.x - (ext_a.x * sign_x),
                               cen_b.y + (ext_b.y * -sign_y) };
        }
    }

    return true;
}

} // namespace physics
} // namespace rdge
