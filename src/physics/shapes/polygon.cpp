#include <rdge/physics/shapes/polygon.hpp>

namespace rdge {
namespace physics {

using namespace rdge::math;

namespace {

    constexpr float HALF_SLOP_SQUARED = math::square(LINEAR_SLOP * 0.5f);

} // anonymous namespace

polygon::polygon (const PolygonData& verts, size_t count)
{
    SDL_assert(3 <= count && count <= MAX_VERTICES);

    // vertex welding
    PolygonData welds;
    size_t weld_count = 0;
    for (size_t i = 0; i < count; ++i)
    {
        auto& v = verts[i];

        bool unique = true;
        for (size_t j = 0; j < weld_count; ++j)
        {
            if ((v - welds[i]).self_dot() < HALF_SLOP_SQUARED)
            {
                unique = false;
                break;
            }
        }

        if (unique)
        {
            welds[weld_count++] = v;
        }
    }

    // new count - check for degenerate
    count = weld_count;
    SDL_assert(3 <= count);

    // convex hull using the gift wrapping algorithm

    // find the rightmost vertex
    size_t right_idx = 0;
    float max_x = welds[0].x;
    for (size_t i = 1; i < count; ++i)
    {
        float x = welds[i].x;
        if (x > max_x || (x == max_x && welds[i].y < welds[right_idx].y))
        {
            max_x = x;
            right_idx = i;
        }
    }

    uint32 hull[MAX_VERTICES];
    uint32 out_count = 0;
    uint32 index = right_idx;

    while (true)
    {
        hull[out_count] = index;
        uint32 next = 0;

        for (size_t i = 1; i < count; ++i)
        {
            if (next == index)
            {
                next = i;
                continue;
            }

            vec2 r = welds[next] - welds[hull[out_count]];
            vec2 v = welds[i] - welds[hull[out_count]];
            float det = cross(r, v);
            if (det < 0.f || (det == 0.f && r.self_dot() < v.self_dot()))
            {
                next = i;
            }
        }

        out_count++;
        index = next;

        if (index == right_idx)
        {
            break;
        }
    }

    // new count - check for degenerate
    count = out_count;
    SDL_assert(3 <= count);

    this->vertices.fill(0.f);
    for (size_t i = 0; i < count; ++i)
    {
        this->vertices[i] = welds[hull[i]];
    }

    this->normals.fill(0.f);
    for (size_t i = 0; i < count; ++i)
    {
        auto other_idx = ((i + i) < count) ? (i + 1) : 0; // wrap if last
        vec2 edge = this->vertices[i] - this->vertices[other_idx];

        this->normals[i] = edge.perp_ccw().normalize();
    }

    // TODO centroid = ...
}

} // namespace physics
} // namespace rdge
