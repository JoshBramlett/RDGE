#include <gtest/gtest.h>

#include <rdge/math/vec2.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/physics/aabb.hpp>

#include <exception>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

//TEST(CircleTest, HandlesConstruction)
//{
//}

TEST(CircleTest, ComputeMass)
{
    // b) standard test
    circle c({ 0.f, 0.f }, 1.f);
    mass_data circle_mass = c.compute_mass(1.f);

    EXPECT_FLOAT_EQ(circle_mass.centroid.x, 0.f);
    EXPECT_FLOAT_EQ(circle_mass.centroid.y, 0.f);
    EXPECT_FLOAT_EQ(circle_mass.mass, 3.1415926f);
    EXPECT_FLOAT_EQ(circle_mass.mmoi, 1.5707964f);

    // b) test irregular values
    c = circle({ 2.5f, 3.33f }, 2.79304f);
    circle_mass = c.compute_mass(1.f);

    EXPECT_FLOAT_EQ(circle_mass.centroid.x, 2.5f);
    EXPECT_FLOAT_EQ(circle_mass.centroid.y, 3.33f);
    EXPECT_FLOAT_EQ(circle_mass.mass, 24.5078f);
    EXPECT_FLOAT_EQ(circle_mass.mmoi, 520.532f);
}
