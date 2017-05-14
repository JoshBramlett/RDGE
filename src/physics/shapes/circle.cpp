#include <rdge/physics/shapes/circle.hpp>

namespace rdge {
namespace physics {

using namespace rdge::math;

bool
circle::intersects_with (const circle& other, collision_manifold& mf) const noexcept
{
    mf.count = 0;
    vec2 d = other.pos - pos;
    float r = radius + other.radius;

    if (d.self_dot() < square(r))
    {
        float l = d.length();
        mf.count = 1;
        mf.depths[0] = r - l;
        mf.contacts[0] = (pos + other.pos) * 0.5f;
        mf.normal = (l != 0.f) ? d * (1.f / l) : vec2(0.f, 1.f);

        return true;
    }

    return false;
}

} // namespace physics
} // namespace rdge