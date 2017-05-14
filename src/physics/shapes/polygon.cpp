#include <rdge/physics/shapes/polygon.hpp>
#include <rdge/internal/logger_macros.hpp>

#include <SDL_assert.h>

namespace rdge {
namespace physics {

using namespace rdge::math;

namespace {

constexpr float HALF_SLOP_SQUARED = math::square(LINEAR_SLOP * 0.5f);

math::vec2
compute_centroid (const polygon::PolygonData& verts, size_t count)
{
    //https://en.wikipedia.org/wiki/Centroid#Centroid_of_a_polygon

    math::vec2 result;
    float area = 0.f;

    for (size_t i = 0; i < count; ++i)
    {
        const auto& a = verts[i];
        const auto& b = verts[((i + 1) < count) ? (i + 1) : 0];

        float partial_area = math::perp_dot(a, b);
        area += partial_area;
        result += (a + b) * partial_area;
    }

    area *= 0.5f;
    result.x /= (6.f * area);
    result.y /= (6.f * area);

    return result;
}

} // anonymous namespace

polygon::polygon (const PolygonData& verts, size_t num_verts)
    : count(num_verts)
{
    SDL_assert(3 <= count && count <= MAX_VERTICES);

    // TODO Box2D provides a graceful fallback mechanisms for degenerate
    //      polygons by creating a 2x2 box.  Seems to be safer to consider
    //      this to be an error case.
    //
    //      For debug builds we'll assert, but this will need to be looked
    //      into further when finally creating a release build.

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
    DLOG_IF(count != weld_count, "Polygon welded vertices");
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
            float det = math::perp_dot(r, v);
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
    DLOG_IF(count != out_count, "Polygon convex hull ignored vertices");
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

    this->centroid = compute_centroid(this->vertices, this->count);
}

polygon::polygon (float he_x, float he_y)
{
    count = 4;
    vertices[0] = { -he_x, -he_y };
    vertices[1] = {  he_x, -he_y };
    vertices[2] = {  he_x,  he_y };
    vertices[3] = { -he_x,  he_y };
    normals[0] = { 0.f, -1.f };
    normals[1] = { 1.f, 0.f };
    normals[2] = { 0.f, 1.f };
    normals[3] = { -1.f, 0.f };
}

polygon::polygon (const vec2& p1, const vec2& p2, const vec2& p3)
    : polygon({{ p1, p2, p3 }}, 3)
{ }

polygon::polygon (const vec2& p1, const vec2& p2, const vec2& p3, const vec2& p4)
    : polygon({{ p1, p2, p3, p4 }}, 4)
{ }

polygon::polygon (const vec2& p1, const vec2& p2, const vec2& p3,
                  const vec2& p4, const vec2& p5)
    : polygon({{ p1, p2, p3, p4, p5 }}, 5)
{ }

mass_data
polygon::compute_mass (float density) const
{
    // polygon is broken down into triangle components, whose calculations are
    // aggregated to find the final result

    mass_data result;
    float area = 0.f;
    for (size_t i = 0; i < count; ++i)
    {
        const auto& a = vertices[i];
        const auto& b = vertices[((i + 1) < count) ? (i + 1) : 0];

        // signed area of the parallelogram component
        float component_area = math::perp_dot(a, b);
        area += component_area;

        // triangle mass moment of inertia: (mass/6.f) * (dot(a, a) + dot(a, b) + dot(b, b))
        float component_mmoi = ((density * component_area * 0.5f) / 6.f) *
                               (math::dot(a, a) + math::dot(a, b) + math::dot(b, b));

        result.centroid += (a + b) * component_area;
        result.mmoi += component_mmoi;
    }

    // components are parallelograms - divide by two for triangles
    area *= 0.5f;

    result.centroid.x /= (6.f * area);
    result.centroid.y /= (6.f * area);
    result.mass = density * area;

    return result;
}

} // namespace physics
} // namespace rdge
