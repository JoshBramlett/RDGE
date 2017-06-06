#include <gtest/gtest.h>

#include <rdge/physics/collision.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/physics/shapes/polygon.hpp>

#include <exception>

namespace {

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

TEST(GJKTest, VerifyIntersection)
{
    // TODO Add more tests

    polygon::PolygonData data1;
    data1[0] = { 4.f, 11.f };
    data1[1] = { 9.f, 9.f };
    data1[2] = { 4.f, 5.f };

    polygon::PolygonData data2;
    data2[0] = { 5.f, 7.f };
    data2[1] = { 12.f, 7.f };
    data2[2] = { 7.f, 3.f };
    data2[3] = { 10.f, 2.f };

    polygon p1(data1, 3);
    polygon p2(data2, 4);

    gjk test1(&p1, &p2);
    EXPECT_TRUE(test1.intersects());

    circle c1({ 4.f, 8.f }, 1.1f);
    circle c2({ 4.f, 8.f }, 1.5f);

    gjk test2(&p2, &c1);
    gjk test3(&p2, &c2);
    EXPECT_FALSE(test2.intersects());
    EXPECT_TRUE(test3.intersects());
}

} // namespace rdge
