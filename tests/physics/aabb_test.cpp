#include <gtest/gtest.h>

#include <rdge/math/vec2.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/physics/collision.hpp>

#include <exception>

namespace {

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;

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

TEST(AABBTest, VerifyOperatorOverloads)
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

TEST(AABBTest, VerifyCoreProperties)
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

TEST(AABBTest, VerifyCalculatedProperties)
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

TEST(AABBTest, VerifyContainsPointResult)
{
    aabb a({ 1.f, 1.f }, { 4.f, 3.f });

    // a) Inside
    EXPECT_TRUE(a.contains(vec2(2.f, 2.f)));

    // b) Outside
    EXPECT_FALSE(a.contains(vec2(0.f, 0.f)));

    // c) Edge
    EXPECT_FALSE(a.contains(vec2(1.f, 2.f)));

    // d) Corner
    EXPECT_FALSE(a.contains(vec2(1.f, 1.f)));
}

TEST(AABBTest, VerifyContainsAABBResult)
{
    aabb a({ 1.f, 1.f }, { 4.f, 3.f });

    // a) Equal values
    EXPECT_FALSE(a.contains(aabb({ 1.f, 1.f }, { 4.f, 3.f })));

    // b) One/two side lengths longer
    EXPECT_FALSE(a.contains(aabb({ 1.f, 1.f }, { 4.f, 4.f })));
    EXPECT_FALSE(a.contains(aabb({ 1.f, 1.f }, { 5.f, 4.f })));

    // c) One/two side lengths shorter
    EXPECT_FALSE(a.contains(aabb({ 1.f, 1.f }, { 3.f, 3.f })));
    EXPECT_FALSE(a.contains(aabb({ 1.f, 1.f }, { 3.f, 2.f })));

    // d) Standard case
    EXPECT_TRUE(a.contains(aabb({ 1.5f, 1.5f }, { 3.5f, 2.5f })));
    EXPECT_FALSE(a.contains(aabb({ 0.5f, 0.5f }, { 4.5f, 3.5f })));

    // d) Intersection/Edge/Corner
    EXPECT_FALSE(a.contains(aabb({ 2.f, 2.f }, { 5.f, 4.f })));
    EXPECT_FALSE(a.contains(aabb({ 1.f, 3.f }, { 4.f, 5.f })));
    EXPECT_FALSE(a.contains(aabb({ 4.f, 3.f }, { 7.f, 6.f })));
}

TEST(AABBTest, VerifyIntersectsWithResult)
{
    // NOTE - Each case is ran twice because intersects_with is overloaded.
    //        The result should be the same for the function that solely
    //        performs the test, as well as the function that generates
    //        a collision manifold.

    aabb a({ 1.f, 1.f }, { 4.f, 3.f });
    collision_manifold dummy;

    // a) Equal values
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 4.f, 3.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 4.f, 3.f }), dummy));

    // b) Shared lo point
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 4.f, 4.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 4.f, 4.f }), dummy));
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 5.f, 4.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 5.f, 4.f }), dummy));
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 3.f, 3.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 3.f, 3.f }), dummy));
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 3.f, 2.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 1.f, 1.f }, { 3.f, 2.f }), dummy));

    // c) Shared corner
    EXPECT_FALSE(a.intersects_with(aabb({ 4.f, 3.f }, { 7.f, 6.f })));
    EXPECT_FALSE(a.intersects_with(aabb({ 4.f, 3.f }, { 7.f, 6.f }), dummy));
    EXPECT_FALSE(a.intersects_with(aabb({ -3.f, 3.f }, { 1.f, 5.f })));
    EXPECT_FALSE(a.intersects_with(aabb({ -3.f, 3.f }, { 1.f, 5.f }), dummy));

    // d) Shared edge
    EXPECT_FALSE(a.intersects_with(aabb({ 1.f, 3.f }, { 4.f, 5.f })));
    EXPECT_FALSE(a.intersects_with(aabb({ 1.f, 3.f }, { 4.f, 5.f }), dummy));
    EXPECT_FALSE(a.intersects_with(aabb({ 4.f, 1.f }, { 7.f, 3.f })));
    EXPECT_FALSE(a.intersects_with(aabb({ 4.f, 1.f }, { 7.f, 3.f }), dummy));

    // e) Contains/Contained
    EXPECT_TRUE(a.intersects_with(aabb({ 1.5f, 1.5f }, { 3.5f, 2.5f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 1.5f, 1.5f }, { 3.5f, 2.5f }), dummy));
    EXPECT_TRUE(a.intersects_with(aabb({ 0.5f, 0.5f }, { 4.5f, 3.5f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 0.5f, 0.5f }, { 4.5f, 3.5f }), dummy));

    // f) Corner (intersection on x and y)
    EXPECT_TRUE(a.intersects_with(aabb({ 2.f, 2.f }, { 5.f, 4.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 2.f, 2.f }, { 5.f, 4.f }), dummy));

    // g) Edge (double intersection on the same axis)
    EXPECT_TRUE(a.intersects_with(aabb({ 2.f, 0.f }, { 3.f, 2.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 2.f, 0.f }, { 3.f, 2.f }), dummy));

    // h) Double edge (quad intersection on the same axis)
    EXPECT_TRUE(a.intersects_with(aabb({ 2.f, 0.f }, { 3.f, 4.f })));
    EXPECT_TRUE(a.intersects_with(aabb({ 2.f, 0.f }, { 3.f, 4.f }), dummy));

    // i) No intersection
    EXPECT_FALSE(a.intersects_with(aabb({ 1.f, 4.f }, { 4.f, 6.f })));
    EXPECT_FALSE(a.intersects_with(aabb({ 1.f, 4.f }, { 4.f, 6.f }), dummy));
}

// Manifold generation details:
//
// Test contains a base AABB and validates the intersections on each of the four
// corners.  In order to validate the normals Two AABBs are tested on each corner,
// where the penetration depth is smaller on the different axes..
//
// Also, manifold data is generated from the perspective of the base object and how
// *it* should resolve itself with the foreign object (meaning a.intersects_with(b) and
// b.intersects_with(a) are not the same), we'll need to test for correctness from
// both vantage points.  Note that if the shorter penetration depth is on the same
// axis the normals should be inverse.

TEST(AABBTest, ValidateManifoldSingleAxis)
{
    aabb a({ 2.f, 2.f }, { 9.f, 8.f });
    collision_manifold mf;

    // Lower (double penetration on x-axis)
    aabb lower_double_on_x({ 5.f, 1.f }, { 6.f, 3.f });

    EXPECT_TRUE(a.intersects_with(lower_double_on_x, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 5.f);
    EXPECT_EQ(mf.contacts[0].y, 2.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, -1.f);

    EXPECT_TRUE(lower_double_on_x.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 5.f);
    EXPECT_EQ(mf.contacts[0].y, 2.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, 1.f);

    // Upper (double penetration on x-axis)
    aabb upper_double_on_x({ 5.f, 7.f }, { 6.f, 9.f });

    EXPECT_TRUE(a.intersects_with(upper_double_on_x, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 5.f);
    EXPECT_EQ(mf.contacts[0].y, 8.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, 1.f);

    EXPECT_TRUE(upper_double_on_x.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 5.f);
    EXPECT_EQ(mf.contacts[0].y, 8.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, -1.f);

    // Lower (double penetration on y-axis)
    aabb lower_double_on_y({ 1.f, 4.f }, { 3.f, 6.f });

    EXPECT_TRUE(a.intersects_with(lower_double_on_y, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 2.f);
    EXPECT_EQ(mf.contacts[0].y, 4.f);
    EXPECT_EQ(mf.normal.x, -1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    EXPECT_TRUE(lower_double_on_y.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 2.f);
    EXPECT_EQ(mf.contacts[0].y, 4.f);
    EXPECT_EQ(mf.normal.x, 1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    // Upper (double penetration on y-axis)
    aabb upper_double_on_y({ 8.f, 4.f }, { 10.f, 6.f });

    EXPECT_TRUE(a.intersects_with(upper_double_on_y, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 9.f);
    EXPECT_EQ(mf.contacts[0].y, 4.f);
    EXPECT_EQ(mf.normal.x, 1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    EXPECT_TRUE(upper_double_on_y.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 9.f);
    EXPECT_EQ(mf.contacts[0].y, 4.f);
    EXPECT_EQ(mf.normal.x, -1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    // Quad penetration on x-axis)
    aabb quad_on_x({ 5.f, 1.f }, { 6.f, 9.f });

    EXPECT_TRUE(a.intersects_with(quad_on_x, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 4.f);
    EXPECT_EQ(mf.contacts[0].x, 5.f);
    EXPECT_EQ(mf.contacts[0].y, 2.f);
    EXPECT_EQ(mf.normal.x, 1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    EXPECT_TRUE(quad_on_x.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 4.f);
    EXPECT_EQ(mf.contacts[0].x, 6.f);
    EXPECT_EQ(mf.contacts[0].y, 2.f);
    EXPECT_EQ(mf.normal.x, 1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    // Quad penetration on y-axis)
    aabb quad_on_y({ 1.f, 4.f }, { 10.f, 6.f });

    EXPECT_TRUE(a.intersects_with(quad_on_y, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 4.f);
    EXPECT_EQ(mf.contacts[0].x, 2.f);
    EXPECT_EQ(mf.contacts[0].y, 4.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, 1.f);

    EXPECT_TRUE(quad_on_y.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 4.f);
    EXPECT_EQ(mf.contacts[0].x, 2.f);
    EXPECT_EQ(mf.contacts[0].y, 6.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, 1.f);
}

TEST(AABBTest, ValidateManifoldDoubleAxis)
{
    aabb a({ 2.f, 2.f }, { 9.f, 8.f });
    collision_manifold mf;

    // Lower Left (penetration on x-axis)
    aabb lower_left_on_x({ 1.f, 1.f }, { 3.f, 4.f });

    EXPECT_TRUE(a.intersects_with(lower_left_on_x, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 2.f);
    EXPECT_EQ(mf.contacts[0].y, 4.f);
    EXPECT_EQ(mf.normal.x, -1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    EXPECT_TRUE(lower_left_on_x.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 3.f);
    EXPECT_EQ(mf.contacts[0].y, 2.f);
    EXPECT_EQ(mf.normal.x, 1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    // Lower Left (penetration on y-axis)
    aabb lower_left_on_y({ 1.f, 1.f }, { 4.f, 3.f });

    EXPECT_TRUE(a.intersects_with(lower_left_on_y, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 4.f);
    EXPECT_EQ(mf.contacts[0].y, 2.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, -1.f);

    EXPECT_TRUE(lower_left_on_y.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 2.f);
    EXPECT_EQ(mf.contacts[0].y, 3.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, 1.f);

    // Upper Left (penetration on x-axis)
    aabb upper_left_on_x({ 1.f, 6.f }, { 3.f, 9.f });

    EXPECT_TRUE(a.intersects_with(upper_left_on_x, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 2.f);
    EXPECT_EQ(mf.contacts[0].y, 6.f);
    EXPECT_EQ(mf.normal.x, -1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    EXPECT_TRUE(upper_left_on_x.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 3.f);
    EXPECT_EQ(mf.contacts[0].y, 8.f);
    EXPECT_EQ(mf.normal.x, 1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    // Upper Left (penetration on y-axis)
    aabb upper_left_on_y({ 1.f, 7.f }, { 4.f, 9.f });

    EXPECT_TRUE(a.intersects_with(upper_left_on_y, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 4.f);
    EXPECT_EQ(mf.contacts[0].y, 8.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, 1.f);

    EXPECT_TRUE(upper_left_on_y.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 2.f);
    EXPECT_EQ(mf.contacts[0].y, 7.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, -1.f);

    // Upper Right (penetration on x-axis)
    aabb upper_right_on_x({ 8.f, 6.f }, { 10.f, 9.f });

    EXPECT_TRUE(a.intersects_with(upper_right_on_x, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 9.f);
    EXPECT_EQ(mf.contacts[0].y, 6.f);
    EXPECT_EQ(mf.normal.x, 1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    EXPECT_TRUE(upper_right_on_x.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 8.f);
    EXPECT_EQ(mf.contacts[0].y, 8.f);
    EXPECT_EQ(mf.normal.x, -1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    // Upper Right (penetration on y-axis)
    aabb upper_right_on_y({ 7.f, 7.f }, { 10.f, 9.f });

    EXPECT_TRUE(a.intersects_with(upper_right_on_y, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 7.f);
    EXPECT_EQ(mf.contacts[0].y, 8.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, 1.f);

    EXPECT_TRUE(upper_right_on_y.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 9.f);
    EXPECT_EQ(mf.contacts[0].y, 7.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, -1.f);

    // Lower Right (penetration on x-axis)
    aabb lower_right_on_x({ 8.f, 1.f }, { 10.f, 4.f });

    EXPECT_TRUE(a.intersects_with(lower_right_on_x, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 9.f);
    EXPECT_EQ(mf.contacts[0].y, 4.f);
    EXPECT_EQ(mf.normal.x, 1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    EXPECT_TRUE(lower_right_on_x.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 8.f);
    EXPECT_EQ(mf.contacts[0].y, 2.f);
    EXPECT_EQ(mf.normal.x, -1.f);
    EXPECT_EQ(mf.normal.y, 0.f);

    // Lower Right (penetration on y-axis)
    aabb lower_right_on_y({ 7.f, 1.f }, { 10.f, 3.f });

    EXPECT_TRUE(a.intersects_with(lower_right_on_y, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 7.f);
    EXPECT_EQ(mf.contacts[0].y, 2.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, -1.f);

    EXPECT_TRUE(lower_right_on_y.intersects_with(a, mf));
    EXPECT_EQ(mf.count, 1);
    EXPECT_EQ(mf.depths[0], 1.f);
    EXPECT_EQ(mf.contacts[0].x, 9.f);
    EXPECT_EQ(mf.contacts[0].y, 3.f);
    EXPECT_EQ(mf.normal.x, 0.f);
    EXPECT_EQ(mf.normal.y, 1.f);
}

} // anonymous namespace
