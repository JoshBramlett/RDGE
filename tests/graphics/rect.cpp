#include <gtest/gtest.h>

#include <rdge/graphics/rect.hpp>
#include <rdge/math/vec2.hpp>

#include <exception>

using namespace rdge;
using namespace rdge::math;

TEST(RectTest, HandlesConstruction)
{
    // a) Standard construction
    rect a({ 1.f, 1.f }, { 4.f, 3.f });
    EXPECT_TRUE(a.pmin.x == 1.f && a.pmin.y == 1.f);
    EXPECT_TRUE(a.pmax.x == 4.f && a.pmax.y == 3.f);

    // b) Mal-ordered standard construction
    rect b({ 4.f, 3.f }, { 1.f, 1.f });
    EXPECT_TRUE(b.pmin.x == 1.f && b.pmin.y == 1.f);
    EXPECT_TRUE(b.pmax.x == 4.f && b.pmax.y == 3.f);

    // c) Point/width/height construction
    rect c({ 4.f, 3.f }, 5.f, 7.f);
    EXPECT_TRUE(c.pmin.x == 4.f && c.pmin.y == 3.f);
    EXPECT_TRUE(c.pmax.x == 9.f && c.pmax.y == 10.f);

    // d) SDL_Rect construction
    screen_rect d(SDL_Rect { 1, 3, 5, 7 });
    EXPECT_TRUE(d.pmin.x == 1 && d.pmin.y == -4);
    EXPECT_TRUE(d.pmax.x == 6 && d.pmax.y == 3);

    // e) SDL_Rect casting
    SDL_Rect e = static_cast<SDL_Rect>(d);
    EXPECT_TRUE(e.x == 1 && e.y == 3 && e.w == 5 && e.h == 7);
}

TEST(RectTest, HandlesAccessors)
{
    rect a({ 1.f, 1.f }, { 4.f, 3.f });
    EXPECT_TRUE(a.width() == 3.f);
    EXPECT_TRUE(a.height() == 2.f);

    EXPECT_TRUE(a.left() == 1.f);
    EXPECT_TRUE(a.right() == 4.f);
    EXPECT_TRUE(a.top() == 3.f);
    EXPECT_TRUE(a.bottom() == 1.f);
}

TEST(RectTest, HandlesCalculatedProperties)
{
    // a) Standard values
    rect a({ 1.f, 1.f }, { 4.f, 3.f });

    math::vec2 ca = a.centroid();
    EXPECT_TRUE(ca.x == 2.5f && ca.y == 2.f);
    math::vec2 hea = a.half_extent();
    EXPECT_TRUE(hea.x == 1.5f && hea.y == 1.f);

    // b) Values around origin
    rect b({ -1.f, -1.f }, { 1.f, 1.f });

    math::vec2 cb = b.centroid();
    EXPECT_TRUE(cb.x == 0.f && cb.y == 0.f);
    math::vec2 heb = b.half_extent();
    EXPECT_TRUE(heb.x == 1.f && heb.y == 1.f);
}

// TODO Add tests for contains() and intersects_with() once it's determined whether
//      they will be inclusive tests or not.  Current collision manifold data below.
//
//      NOTE - make sure to run through the ringer with edge cases
//TEST(RectTest, HandlesContains)
//{

//}

//TEST(RectTest, HandlesIntersectsWith)
//{

//}

//    colliding rect:
//      pmin=[2, 2]
//      pmax=[9, 8]
//
//    Lower Left (x-axis)
//      pmin=[1, 1]
//      pmax=[3, 4]
//      collision=true
//      count=1
//      depth=1
//      contact=[3, 2]
//      normal=[1, 0]
//
//    Lower Left (y-axis)    collision=true
//      pmin=[1, 1]
//      pmax=[4, 3]
//      count=1
//      depth=1
//      contact=[2, 3]
//      normal=[0, 1]
//
//    Upper Left (x-axis)    collision=true
//      pmin=[1, 6]
//      pmax=[3, 9]
//      count=1
//      depth=1
//      contact=[3, 8]
//      normal=[1, 0]
//
//    Upper Left (y-axis)    collision=true
//      pmin=[1, 7]
//      pmax=[4, 9]
//      count=1
//      depth=1
//      contact=[2, 7]
//      normal=[0, -1]
//
//    Upper Right (x-axis)    collision=true
//      pmin=[8, 6]
//      pmax=[10, 9]
//      count=1
//      depth=1
//      contact=[8, 8]
//      normal=[-1, 0]
//
//    Upper Right (y-axis)    collision=true
//      pmin=[7, 7]
//      pmax=[10, 9]
//      count=1
//      depth=1
//      contact=[9, 7]
//      normal=[0, -1]
//
//    Lower Right (x-axis)    collision=true
//      pmin=[8, 1]
//      pmax=[10, 4]
//      count=1
//      depth=1
//      contact=[8, 2]
//      normal=[-1, 0]
//
//    Lower Right (y-axis)    collision=true
//      pmin=[7, 1]
//      pmax=[10, 3]
//      count=1
//      depth=1
//      contact=[9, 3]
//      normal=[0, 1]
