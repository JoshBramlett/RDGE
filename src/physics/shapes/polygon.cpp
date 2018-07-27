#include <rdge/physics/shapes/polygon.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/util/logger.hpp>

#include <SDL_assert.h>

namespace rdge {
namespace physics {

using namespace rdge::math;

namespace {

constexpr float HALF_SLOP_SQUARED = math::square(LINEAR_SLOP * 0.5f);

math::vec2
compute_centroid (const polygon::PolygonData& verts, size_t count)
{
    // TODO this is done during the mass calculation as well.  seems redundant.
    //https://en.wikipedia.org/wiki/Centroid#Centroid_of_a_polygon

    math::vec2 result;
    float area = 0.f;

    for (size_t i = 0; i < count; i++)
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
    for (size_t i = 0; i < count; i++)
    {
        auto& v = verts[i];

        bool unique = true;
        for (size_t j = 0; j < weld_count; ++j)
        {
            if ((v - welds[j]).self_dot() < HALF_SLOP_SQUARED)
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
    DLOG_IF(count != weld_count) << "Polygon welded vertices";
    count = weld_count;
    SDL_assert(3 <= count);

    // convex hull using the gift wrapping algorithm

    // find the rightmost vertex
    size_t right_idx = 0;
    float max_x = welds[0].x;
    for (size_t i = 1; i < count; i++)
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

        for (size_t i = 1; i < count; i++)
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
    DLOG_IF(count != weld_count) << "Polygon convex hull ignored vertices";
    count = out_count;
    SDL_assert(3 <= count);

    this->vertices.fill(0.f);
    for (size_t i = 0; i < count; ++i)
    {
        this->vertices[i] = welds[hull[i]];
    }

    // polygon is built ccw, perp() used b/c edge normals point outwards
    this->normals.fill(0.f);
    for (size_t i = 0; i < count; i++)
    {
        auto other_idx = ((i + 1) < count) ? (i + 1) : 0;
        vec2 edge = this->vertices[i] - this->vertices[other_idx];

        this->normals[i] = edge.perp().normalize();
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

polygon::polygon (float he_x, float he_y, const math::vec2& center, float angle)
    : polygon(he_x, he_y)
{
    centroid = center;
    iso_transform xf(center, angle);

    for (size_t i = 0; i < count; ++i)
    {
        vertices[i] = xf.to_world(vertices[i]);
        normals[i] = xf.rot.rotate(normals[i]);
    }
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

bool
polygon::contains (const vec2& point) const
{
    SDL_assert(count >= 3);

    for (size_t i = 0; i < count; i++)
    {
        if (dot(normals[i], point - vertices[i]) >= 0.f)
        {
            return false;
        }
    }

    return true;
}

aabb
polygon::compute_aabb (void) const
{
    SDL_assert(count >= 3);

    vec2 lo = vertices[0];
    vec2 hi = lo;

    for (size_t i = 1; i < count; i++)
    {
        lo.x = std::min(lo.x, vertices[i].x);
        lo.y = std::min(lo.y, vertices[i].y);
        hi.x = std::max(hi.x, vertices[i].x);
        hi.y = std::max(hi.y, vertices[i].y);
    }

    return aabb(lo - AABB_PADDING, hi + AABB_PADDING);
}

aabb
polygon::compute_aabb (const iso_transform& xf) const
{
    SDL_assert(count >= 3);

    vec2 lo = xf.to_world(vertices[0]);
    vec2 hi = lo;

    for (size_t i = 1; i < count; i++)
    {
        vec2 p = xf.to_world(vertices[i]);
        lo.x = std::min(lo.x, p.x);
        lo.y = std::min(lo.y, p.y);
        hi.x = std::max(hi.x, p.x);
        hi.y = std::max(hi.y, p.y);
    }

    return aabb(lo - AABB_PADDING, hi + AABB_PADDING);
}

mass_data
polygon::compute_mass (float density) const
{
    SDL_assert(count >= 3);

    // polygon is broken down into triangle components, whose calculations are
    // aggregated to find the final result

    mass_data result;
    float area = 0.f;
    for (size_t i = 0; i < count; i++)
    {
        const auto& a = vertices[i];
        const auto& b = vertices[((i + 1) < count) ? (i + 1) : 0];

        // signed area of the parallelogram component
        float component_area = perp_dot(a, b);
        area += component_area;

        // triangle mass moment of inertia: (mass/6.f) * (dot(a, a) + dot(a, b) + dot(b, b))
        float component_mmoi = ((density * component_area * 0.5f) / 6.f) *
                               (dot(a, a) + dot(a, b) + dot(b, b));

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

bool
polygon::intersects_with (const ishape* other) const
{
    gjk test(this, other);
    return test.intersects();
}

bool
polygon::intersects_with (const ishape* other, collision_manifold& mf) const
{
    if (other->type() == ShapeType::POLYGON)
    {
        return intersects_with(*static_cast<const polygon*>(other), mf);
    }

    return intersects(*this, *static_cast<const circle*>(other), mf);
}

bool
polygon::intersects_with (const polygon& other, collision_manifold& mf) const noexcept
{
    // Uses SAT to perform the test and generate the manifold.  Based on:
    //   - Box2D b2CollidePolygons()
    //   - tinyc2 c2PolytoPolyManifold()

    mf.count = 0;

    // bail if separating axis is found
    auto sep_a = max_separation(other);
    if (sep_a.first > 0.f)
    {
        return false;
    }

    auto sep_b = other.max_separation(*this);
    if (sep_b.first > 0.f)
    {
        return false;
    }

    // define reference and incident polygons
    const polygon* ref_shape = nullptr;
    const polygon* inc_shape = nullptr;
    uint32 ref_edge = 0;
    uint32 inc_edge = 0;

    if (sep_b.first > sep_a.first + RELATIVE_TOLERANCE)
    {
        ref_shape = &other;
        inc_shape = this;
        ref_edge = sep_b.second;
        mf.flip = true;
    }
    else
    {
        ref_shape = this;
        inc_shape = &other;
        ref_edge = sep_a.second;
        mf.flip = false;
    }

    // find the incident edge
    {
        const auto& ref_n = ref_shape->normals[ref_edge];
        float d_min = math::dot(ref_n, inc_shape->normals[0]);
        for (size_t i = 1; i < inc_shape->count; i++)
        {
            float d = math::dot(ref_n, inc_shape->normals[i]);
            if (d < d_min)
            {
                d_min = d;
                inc_edge = i;
            }
        }
    }

    // build the clip vertices for the incident edge
    math::vec2 inc_vertices[2];
    inc_vertices[0] = inc_shape->vertices[inc_edge];
    uint32 next_v = ((inc_edge + 1) < inc_shape->count) ? (inc_edge + 1) : 0;
    inc_vertices[1] = inc_shape->vertices[next_v];

    // create two orthogonal planes from the reference edge attached to the end points
    math::vec2 ref_vertices[2];
    ref_vertices[0] = ref_shape->vertices[ref_edge];
    next_v = ((ref_edge + 1) < ref_shape->count) ? ref_edge + 1 : 0;
    ref_vertices[1] = ref_shape->vertices[next_v];

    auto tangent = (ref_vertices[1] - ref_vertices[0]).normalize();
    half_plane left = { -tangent, math::dot(-tangent, ref_vertices[0]) };
    half_plane right = { tangent, math::dot(tangent, ref_vertices[1]) };

    // clip incident vertices to reference planes
    {
        int32 num_out = 0;
        float d0 = distance(left, inc_vertices[0]);
        float d1 = distance(left, inc_vertices[1]);

        // points are behind the plane
        if (d0 <= 0.f) { num_out++; }
        if (d1 <= 0.f) { num_out++; }

        // points are on different sides of the plane
        if ((d0 * d1) < 0.f)
        {
            inc_vertices[num_out++] = inc_vertices[0] +
                                      (d0 / (d0 - d1)) * (inc_vertices[1] - inc_vertices[0]);
        }

        if (num_out < 2)
        {
            return false;
        }
    }

    {
        int32 num_out = 0;
        float d0 = distance(right, inc_vertices[0]);
        float d1 = distance(right, inc_vertices[1]);

        if (d0 <= 0.f) { num_out++; }
        if (d1 <= 0.f) { num_out++; }

        if ((d0 * d1) < 0.f)
        {
            inc_vertices[num_out++] = inc_vertices[0] +
                                      (d0 / (d0 - d1)) * (inc_vertices[1] - inc_vertices[0]);
        }

        if (num_out < 2)
        {
            return false;
        }
    }

    half_plane penetration_plane = {
        tangent.perp_ccw(),
        math::dot(tangent.perp_ccw(), ref_vertices[0])
    };

    int32 num_points = 0;
    for (int32 i = 0; i < 2; i++)
    {
        auto p = inc_vertices[i];
        float d = distance(penetration_plane, p);
        if (d < 0.f)
        {
            mf.contacts[num_points] = p;
            mf.depths[num_points] = -d;
            num_points++;
        }
    }

    mf.count = num_points;
    mf.normal = penetration_plane.normal;
    mf.plane = (ref_vertices[0] + ref_vertices[1]) * 0.5f;
    return true;
}

std::ostream& operator<< (std::ostream& os, const polygon& p)
{
    if (p.count == 0)
    {
        return os << "[ polygon: count=0 ]\n";
    }

    os << "polygon: ["
       << "\n  count=" << p.count
       << "\n  centroid=" << p.centroid;

    for (size_t i = 0; i < p.count; i++)
    {
        os << "\n  vertices[" << i << "]=" << p.vertices[i]
           << " normals[" << i << "]=" << p.normals[i];
    }

    return os << "\n]\n";
}

} // namespace physics
} // namespace rdge
