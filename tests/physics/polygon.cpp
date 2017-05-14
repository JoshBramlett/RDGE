#include <gtest/gtest.h>

#include <rdge/math/vec2.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/physics/shapes/polygon.hpp>
#include <rdge/physics/aabb.hpp>

#include <exception>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

//TEST(PolygonTest, HandlesConstruction)
//{
//}

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
    EXPECT_EQ(triangle_mass.mass, 4.5f);
    EXPECT_EQ(triangle_mass.mmoi, 18.5625f);

    // b) positive points
    triangle = polygon({ 1.f, 1.f }, { 2.5f, 4.f }, { 4.f, 1.f });
    triangle_mass = triangle.compute_mass(1.f);

    EXPECT_EQ(triangle_mass.centroid, vec2(2.5f, 2.f));
    EXPECT_EQ(triangle_mass.mass, 4.5f);
    EXPECT_EQ(triangle_mass.mmoi, 50.0625f);

    // c) negative points
    triangle = polygon({ -1.f, -1.f }, { 0.5f, 2.f }, { 2.f, -1.f });
    triangle_mass = triangle.compute_mass(1.f);

    EXPECT_EQ(triangle_mass.centroid, vec2(0.5f, 0.f));
    EXPECT_EQ(triangle_mass.mass, 4.5f);
    EXPECT_EQ(triangle_mass.mmoi, 5.0625f);

    // d) test pentagon
    polygon pentagon({ 0.f, 0.f }, { -1.f, 2.f }, { 1.5f, 3.f }, { 4.f, 2.f }, { 3.f, 0.f });
    mass_data pentagon_mass = pentagon.compute_mass(1.f);

    EXPECT_FLOAT_EQ(pentagon_mass.centroid.x, 1.5f);
    EXPECT_FLOAT_EQ(pentagon_mass.centroid.y, 1.3809524f);
    EXPECT_EQ(pentagon_mass.mass, 10.5f);
    EXPECT_EQ(pentagon_mass.mmoi, 63.3125f);

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
    EXPECT_EQ(irregular_mass.mass, 6.f);
    EXPECT_EQ(irregular_mass.mmoi, 8.5f);
}
