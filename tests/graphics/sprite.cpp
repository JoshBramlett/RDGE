#include <gtest/gtest.h>

#include <rdge/graphics/isprite.hpp>
#include <rdge/math/functions.hpp>

using namespace rdge;
using namespace rdge::math;

TEST(SmartZIndexTest, HandlesLayerDepthConversion)
{
    using test_mask = smart_zindex<LayerDepthOffset::value>;

    // 1) Zero value
    EXPECT_TRUE(fp_eq(test_mask::convert(0), 0.f));

    // 2) Single digit
    EXPECT_TRUE(fp_eq(test_mask::convert(1), 0.01f));

    // 2) Double digit
    EXPECT_TRUE(fp_eq(test_mask::convert(11), 0.11f));

    // 2) Overflow
    EXPECT_TRUE(fp_eq(test_mask::convert(101), 0.99f));
}

TEST(SmartZIndexTest, HandlesSpriteDepthConversion)
{
    using test_mask = smart_zindex<SpriteDepthOffset::value>;

    // 1) Zero value
    EXPECT_TRUE(fp_eq(test_mask::convert(0), 0.f));

    // 2) Single digit
    EXPECT_TRUE(fp_eq(test_mask::convert(1), 0.0001f));

    // 2) Double digit
    EXPECT_TRUE(fp_eq(test_mask::convert(11), 0.0011f));

    // 2) Overflow
    EXPECT_TRUE(fp_eq(test_mask::convert(101), 0.0099f));
}
