#include <gtest/gtest.h>

#include <rdge/math/functions.hpp>
#include <cmath>

using namespace RDGE::Math;

TEST(FloatingPointEqualTest, HandlesIrrational)
{
    EXPECT_TRUE(fp_eq(NAN, NAN));
    EXPECT_TRUE(fp_eq(INFINITY, -INFINITY));
}

TEST(FloatingPointEqualTest, HandlesFloat)
{
    //FLT_EPSILON = 0.00000011920
    float f_a     = 0.0000001192;
    float f_true  = 0.0000001692;
    float f_false = 0.0000003192;
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
