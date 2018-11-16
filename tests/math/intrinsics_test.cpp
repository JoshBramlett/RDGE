#include <gtest/gtest.h>

#include <rdge/core.hpp>
#include <rdge/math/intrinsics.hpp>
#include <cmath>

using namespace rdge;
using namespace rdge::math;

namespace {

TEST(FloatingPointEqualTest, HandlesIrrational)
{
    EXPECT_TRUE(fp_eq(qnan32, qnan32));
    EXPECT_TRUE(fp_eq(INFINITY, -INFINITY));
}

TEST(FloatingPointEqualTest, HandlesFloat)
{
    //FLT_EPSILON = 0.00000011920
    float f_a     = 0.0000001192f;
    float f_true  = 0.0000001692f;
    float f_false = 0.0000003192f;
    EXPECT_TRUE(fp_eq(f_a, f_true));
    EXPECT_FALSE(fp_eq(f_a, f_false));
}

TEST(FloatingPointEqualTest, HandlesDouble)
{
    //DBL_EPSILON  = 0.00000000000000022204
    double d_a     = 0.00000000000000022204;
    double d_true  = 0.00000000000000032204;
    double d_false = 0.00000000000000052204;
    EXPECT_TRUE(fp_eq(d_a, d_true));
    EXPECT_FALSE(fp_eq(d_a, d_false));
}

TEST(FloatingPointEqualTest, HandlesLongDouble)
{
    //LDBL_EPSILON       = 0.00000000000000000010842
    long double ld_a     = 0.00000000000000000010842;
    long double ld_true  = 0.00000000000000000015842;
    long double ld_false = 0.00000000000000000030842;
    EXPECT_TRUE(fp_eq(ld_a, ld_true));
    EXPECT_FALSE(fp_eq(ld_a, ld_false));
}

TEST(PowerOfTwoTest, HandlesAll)
{
    // 1) Success
    EXPECT_TRUE(is_pot(128));

    EXPECT_FALSE(is_pot(0));    // zero
    EXPECT_FALSE(is_pot(11));   // odd
    EXPECT_FALSE(is_pot(10));   // non-pot even
}

TEST(FloatingPointIsZero, HandlesAll)
{
    EXPECT_TRUE(is_zero(0.f));
    EXPECT_TRUE(is_zero(-0.f));
    EXPECT_FALSE(is_zero(0.f + std::numeric_limits<float>::epsilon()));
    EXPECT_FALSE(is_zero(0.f - std::numeric_limits<float>::epsilon()));
    EXPECT_FALSE(is_zero(-0.f + std::numeric_limits<float>::epsilon()));
    EXPECT_FALSE(is_zero(-0.f - std::numeric_limits<float>::epsilon()));
}

TEST(LSBTest, HandlesAll)
{
    EXPECT_EQ(math::lsb(24), 4);
    EXPECT_EQ(math::lsb(8), 4);

    EXPECT_EQ(math::lsb(20), 3);
    EXPECT_EQ(math::lsb(12), 3);
    EXPECT_EQ(math::lsb(4), 3);

    EXPECT_EQ(math::lsb(129), 1);
    EXPECT_EQ(math::lsb(65), 1);
    EXPECT_EQ(math::lsb(33), 1);
    EXPECT_EQ(math::lsb(1), 1);

    EXPECT_EQ(math::lsb(0), 0);
}

} // anonymous namespace
