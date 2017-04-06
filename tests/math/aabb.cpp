#include <gtest/gtest.h>

#include <rdge/math/aabb.hpp>
#include <rdge/math/vec2.hpp>

#include <exception>

using namespace rdge;
using namespace rdge::math;

TEST(AABBTest, HandlesConstruction)
{
    // a) Standard construction
    aabb a({ 1.f, 1.f }, { 4.f, 3.f });
    EXPECT_TRUE(a.lo.x == 1.f && a.lo.y == 1.f);
    EXPECT_TRUE(a.hi.x == 4.f && a.hi.y == 3.f);

    // b) Mal-ordered standard construction
    aabb b({ 4.f, 3.f }, { 1.f, 1.f });
    EXPECT_TRUE(b.lo.x == 1.f && b.lo.y == 1.f);
    EXPECT_TRUE(b.hi.x == 4.f && b.hi.y == 3.f);

    // c) Point/width/height construction
    aabb c({ 4.f, 3.f }, 5.f, 7.f);
    EXPECT_TRUE(c.lo.x == 4.f && c.lo.y == 3.f);
    EXPECT_TRUE(c.hi.x == 9.f && c.hi.y == 10.f);
}

TEST(AABBTest, HandlesOperatorOverloads)
{
    // a) Equality/inequality operators
    aabb a({ 1.f, 1.f }, { 4.f, 3.f });
    aabb b({ 1.f, 1.f }, { 4.f, 3.f });
    aabb c({ 1.f, 2.f }, { 4.f, 3.f });
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
}

TEST(AABBTest, HandlesCoreProperties)
{
    // a) Size
    aabb a({ 1.f, 1.f }, { 4.f, 3.f });
    EXPECT_TRUE(a.width() == 3.f);
    EXPECT_TRUE(a.height() == 2.f);

    // b) Edges
    EXPECT_TRUE(a.left() == 1.f);
    EXPECT_TRUE(a.right() == 4.f);
    EXPECT_TRUE(a.top() == 3.f);
    EXPECT_TRUE(a.bottom() == 1.f);

    // c) Validity
    EXPECT_TRUE(a.is_valid());
    a.lo.x = 5.f;
    EXPECT_FALSE(a.is_valid());
}

TEST(AABBTest, HandlesCalculatedProperties)
{
    // a) Standard values
    aabb a({ 1.f, 1.f }, { 4.f, 3.f });

    vec2 ca = a.centroid();
    EXPECT_TRUE(ca.x == 2.5f && ca.y == 2.f);
    vec2 hea = a.half_extent();
    EXPECT_TRUE(hea.x == 1.5f && hea.y == 1.f);

    // b) Values around origin
    aabb b({ -1.f, -1.f }, { 1.f, 1.f });

    vec2 cb = b.centroid();
    EXPECT_TRUE(cb.x == 0.f && cb.y == 0.f);
    vec2 heb = b.half_extent();
    EXPECT_TRUE(heb.x == 1.f && heb.y == 1.f);
}

TEST(AABBTest, HandlesContainsPoint)
{
    aabb a({ 1.f, 1.f }, { 4.f, 3.f });

    // a) Inside
    EXPECT_TRUE(a.contains(vec2(2.f, 2.f)));

    // b) Outside
    EXPECT_FALSE(a.contains(vec2(0.f, 0.f)));

    // c) Edge
    EXPECT_TRUE(a.contains(vec2(1.f, 2.f)));

    // d) Corner
    EXPECT_TRUE(a.contains(vec2(1.f, 1.f)));
}

TEST(AABBTest, HandlesContainsAABB)
{
    aabb a({ 1.f, 1.f }, { 4.f, 3.f });

    // a) Equal values
    EXPECT_TRUE(a.contains(aabb({ 1.f, 1.f }, { 4.f, 3.f })));

    // b) One/two side lengths longer
    EXPECT_FALSE(a.contains(aabb({ 1.f, 1.f }, { 4.f, 4.f })));
    EXPECT_FALSE(a.contains(aabb({ 1.f, 1.f }, { 5.f, 4.f })));

    // c) One/two side lengths shorter
    EXPECT_TRUE(a.contains(aabb({ 1.f, 1.f }, { 3.f, 3.f })));
    EXPECT_TRUE(a.contains(aabb({ 1.f, 1.f }, { 3.f, 2.f })));

    // d) Standard case
    EXPECT_TRUE(a.contains(aabb({ 1.5f, 1.5f }, { 3.5f, 2.5f })));
    EXPECT_FALSE(a.contains(aabb({ 0.5f, 0.5f }, { 4.5f, 3.5f })));

    // TODO false - partial intersection
    //      false - shared edge
    //      false - shared corner
}

TEST(AABBTest, HandlesIntersectsWith)
{
    aabb a({ 1.f, 1.f }, { 4.f, 3.f });

    // a) Equal values
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 4.f, 3.f })));

    // b) Shared lo point
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 4.f, 4.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 5.f, 4.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 3.f, 3.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 3.f, 2.f })));

    // c) Shared corner
    EXPECT_TRUE(a.intersects_with(aabb({ 4.f, 3.f }, { 4.f, 3.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ -3.f, 3.f }, { 1.f, 5.f })));

    // d) Shared edge
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 3.f }, { 4.f, 5.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 4.f, 1.f }, { 7.f, 3.f })));

    // e) Contains/Contained
    EXPECT_TRUE(a.intersects_with(aabb({ 1.5f, 1.5f }, { 3.5f, 2.5f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 0.5f, 0.5f }, { 4.5f, 3.5f })));

    // f) Standard case
    EXPECT_TRUE(a.intersects_with(aabb({ 2.f, 2.f }, { 5.f, 4.f })));
    EXPECT_FALSE(a.intersects_with(aabb({ 1.f, 4.f }, { 4.f, 6.f })));
}

// TODO Add tests for contains() and intersects_with() once it's determined whether
//      they will be inclusive tests or not.  Current collision manifold data below.
//
//      NOTE - make sure to run through the ringer with edge cases
//    colliding rect:
//      lo=[2, 2]
//      hi=[9, 8]
//
//    Lower Left (x-axis)
//      lo=[1, 1]
//      hi=[3, 4]
//      collision=true
//      count=1
//      depth=1
//      contact=[3, 2]
//      normal=[1, 0]
//
//    Lower Left (y-axis)    collision=true
//      lo=[1, 1]
//      hi=[4, 3]
//      count=1
//      depth=1
//      contact=[2, 3]
//      normal=[0, 1]
//
//    Upper Left (x-axis)    collision=true
//      lo=[1, 6]
//      hi=[3, 9]
//      count=1
//      depth=1
//      contact=[3, 8]
//      normal=[1, 0]
//
//    Upper Left (y-axis)    collision=true
//      lo=[1, 7]
//      hi=[4, 9]
//      count=1
//      depth=1
//      contact=[2, 7]
//      normal=[0, -1]
//
//    Upper Right (x-axis)    collision=true
//      lo=[8, 6]
//      hi=[10, 9]
//      count=1
//      depth=1
//      contact=[8, 8]
//      normal=[-1, 0]
//
//    Upper Right (y-axis)    collision=true
//      lo=[7, 7]
//      hi=[10, 9]
//      count=1
//      depth=1
//      contact=[9, 7]
//      normal=[0, -1]
//
//    Lower Right (x-axis)    collision=true
//      lo=[8, 1]
//      hi=[10, 4]
//      count=1
//      depth=1
//      contact=[8, 2]
//      normal=[-1, 0]
//
//    Lower Right (y-axis)    collision=true
//      lo=[7, 1]
//      hi=[10, 3]
//      count=1
//      depth=1
//      contact=[9, 3]
//      normal=[0, 1]
