#include <gtest/gtest.h>

#include <rdge/math/vec2.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/physics/shapes/polygon.hpp>
#include <rdge/physics/aabb.hpp>

#include <exception>

namespace {

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

TEST(PolygonTest, HandlesConstruction)
{
    polygon tri_a({ 2.f, 7.f }, { 4.f, 2.f }, { 8.f, 7.f });

    //tri_a: [
        //polygon: count=3
        //centroid=[4.66667, 5.33333]
        //vertices[0]=[8, 7] normals[0]=[-0, 1]
        //vertices[1]=[2, 7] normals[1]=[-0.928477, -0.371391]
        //vertices[2]=[4, 2] normals[2]=[0.780869, -0.624695]
    //]

    EXPECT_EQ(tri_a.count, 3);
    EXPECT_FLOAT_EQ(tri_a.centroid.x, 4.6666665f);
    EXPECT_FLOAT_EQ(tri_a.centroid.y, 5.3333335f);

    EXPECT_EQ(tri_a.vertices[0], vec2(8.f, 7.f));
    EXPECT_EQ(tri_a.vertices[1], vec2(2.f, 7.f));
    EXPECT_EQ(tri_a.vertices[2], vec2(4.f, 2.f));

    EXPECT_FLOAT_EQ(tri_a.normals[0].x, 0.f);
    EXPECT_FLOAT_EQ(tri_a.normals[0].y, 1.f);
    EXPECT_FLOAT_EQ(tri_a.normals[1].x, -0.92847669f);
    EXPECT_FLOAT_EQ(tri_a.normals[1].y, -0.37139067f);
    EXPECT_FLOAT_EQ(tri_a.normals[2].x, 0.780869f);
    EXPECT_FLOAT_EQ(tri_a.normals[2].y, -0.624695f);

    polygon tri_b({ 5.f, 9.f }, { 9.f, 3.f }, { 11.f, 10.f });

    //tri_b: [
        //polygon: count=3
        //centroid=[8.33333, 7.33333]
        //vertices[0]=[11, 10] normals[0]=[-0.164399, 0.986394]
        //vertices[1]=[5, 9] normals[1]=[-0.83205, -0.5547]
        //vertices[2]=[9, 3] normals[2]=[0.961524, -0.274721]
    //]

    EXPECT_EQ(tri_b.count, 3);
    EXPECT_FLOAT_EQ(tri_b.centroid.x, 8.3333335f);
    EXPECT_FLOAT_EQ(tri_b.centroid.y, 7.3333335f);

    EXPECT_EQ(tri_b.vertices[0], vec2(11.f, 10.f));
    EXPECT_EQ(tri_b.vertices[1], vec2(5.f, 9.f));
    EXPECT_EQ(tri_b.vertices[2], vec2(9.f, 3.f));

    EXPECT_FLOAT_EQ(tri_b.normals[0].x, -0.164399f);
    EXPECT_FLOAT_EQ(tri_b.normals[0].y, 0.986394f);
    EXPECT_FLOAT_EQ(tri_b.normals[1].x, -0.8320503f);
    EXPECT_FLOAT_EQ(tri_b.normals[1].y, -0.5547f);
    EXPECT_FLOAT_EQ(tri_b.normals[2].x, 0.961524f);
    EXPECT_FLOAT_EQ(tri_b.normals[2].y, -0.274721f);
}

TEST(PolygonTest, VerifyContainsPointResult)
{
    polygon tri_a({ 2.f, 7.f }, { 4.f, 2.f }, { 8.f, 7.f });

    // a) Inside
    EXPECT_TRUE(tri_a.contains({ 4.f, 5.f }));

    // b) Outside
    EXPECT_FALSE(tri_a.contains({ 1.f, 7.f }));

    // c) Edge
    EXPECT_FALSE(tri_a.contains({ 3.f, 7.f }));

    // d) Corner
    EXPECT_FALSE(tri_a.contains({ 2.f, 7.f }));
}

TEST(PolygonTest, ComputeMass)
{
    // Firsts three tests Test different points b/c the computation uses
    // signed area (meaning we must ensure negative and positive values
    // have consistent input).

    // a) point at origin
    polygon triangle({ 0.f, 0.f }, { 1.5f, 3.f }, { 3.f, 0.f });
    mass_data triangle_mass = triangle.compute_mass(1.f);

    // fyi - mmoi without shift to origin: 3.9375
    EXPECT_EQ(triangle_mass.centroid, vec2(1.5f, 1.f));
    EXPECT_FLOAT_EQ(triangle_mass.mass, 4.5f);
    EXPECT_FLOAT_EQ(triangle_mass.mmoi, 18.5625f);

    // b) positive points
    triangle = polygon({ 1.f, 1.f }, { 2.5f, 4.f }, { 4.f, 1.f });
    triangle_mass = triangle.compute_mass(1.f);

    EXPECT_EQ(triangle_mass.centroid, vec2(2.5f, 2.f));
    EXPECT_FLOAT_EQ(triangle_mass.mass, 4.5f);
    EXPECT_FLOAT_EQ(triangle_mass.mmoi, 50.0625f);

    // c) negative points
    triangle = polygon({ -1.f, -1.f }, { 0.5f, 2.f }, { 2.f, -1.f });
    triangle_mass = triangle.compute_mass(1.f);

    EXPECT_EQ(triangle_mass.centroid, vec2(0.5f, 0.f));
    EXPECT_FLOAT_EQ(triangle_mass.mass, 4.5f);
    EXPECT_FLOAT_EQ(triangle_mass.mmoi, 5.0625f);

    // d) test pentagon
    polygon pentagon({ 0.f, 0.f }, { -1.f, 2.f }, { 1.5f, 3.f }, { 4.f, 2.f }, { 3.f, 0.f });
    mass_data pentagon_mass = pentagon.compute_mass(1.f);

    EXPECT_FLOAT_EQ(pentagon_mass.centroid.x, 1.5f);
    EXPECT_FLOAT_EQ(pentagon_mass.centroid.y, 1.3809524f);
    EXPECT_FLOAT_EQ(pentagon_mass.mass, 10.5f);
    EXPECT_FLOAT_EQ(pentagon_mass.mmoi, 63.3125f);

    // d) test irregular polygon
    polygon::PolygonData data;
    data[0] = { -1.f, 2.f };
    data[1] = { -1.f, 0.f };
    data[2] = { 0.f, -3.f };
    data[3] = { 1.f, 0.f };
    data[4] = { 1.f, 1.f };
    polygon irregular(data, 5);
    mass_data irregular_mass = irregular.compute_mass(1.f);

    EXPECT_FLOAT_EQ(irregular_mass.centroid.x, -0.055555556f);
    EXPECT_FLOAT_EQ(irregular_mass.centroid.y, -0.11111111f);
    EXPECT_FLOAT_EQ(irregular_mass.mass, 6.f);
    EXPECT_FLOAT_EQ(irregular_mass.mmoi, 8.5f);
}

TEST(PolygonTest, VerifyManifoldPolygonPolygon)
{
    polygon tri_a({ 2.f, 7.f }, { 4.f, 2.f }, { 8.f, 7.f });
    polygon tri_b({ 5.f, 9.f }, { 9.f, 3.f }, { 11.f, 10.f });

    collision_manifold mf;
    bool result = tri_a.intersects_with(tri_b, mf);
    EXPECT_TRUE(result);

    // TODO Further validate collision manifolds.  The count, contact point,
    //      and flip_dominant are validated against Box2D.  The normal
    //      validates to the localNormal of the Box2D manifold.  There is
    //      no depth to validate against.

    //mf: [
        //manifold: count=1
        //normal=[-0.83205, -0.5547]
        //flip_dominant=true
        //contacts[0]=[8, 7] depths[0]=1.38675
    //]
}

} // anonymous namespace
