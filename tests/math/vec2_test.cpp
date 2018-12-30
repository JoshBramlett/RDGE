#include <gtest/gtest.h>

#include <rdge/math/vec2.hpp>

using namespace rdge;
using namespace rdge::math;

namespace {

TEST(Vec2FloatingPointTest, HandlesStaticMembers)
{
    {
        auto test = rdge::math::vec2::ZERO;
        EXPECT_TRUE(is_zero(test.x));
        EXPECT_TRUE(is_zero(test.y));
        EXPECT_TRUE(test.is_zero());
    }

    {
        auto test = rdge::math::vec2::X;
        EXPECT_TRUE(fp_eq(test.x, 1.f));
        EXPECT_TRUE(is_zero(test.y));
        EXPECT_FALSE(test.is_zero());
    }

    {
        auto test = rdge::math::vec2::Y;
        EXPECT_TRUE(is_zero(test.x));
        EXPECT_TRUE(fp_eq(test.y, 1.f));
        EXPECT_FALSE(test.is_zero());
    }
}

TEST(Vec2IntegralTest, HandlesStaticMembers)
{
    {
        auto test = rdge::math::uivec2::ZERO;
        EXPECT_TRUE(is_zero(test.x));
        EXPECT_TRUE(is_zero(test.y));
        EXPECT_TRUE(test.is_zero());
    }
}

} // anonymous namespace
