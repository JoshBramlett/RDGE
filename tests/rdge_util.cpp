#include <gtest/gtest.h>

#include <cmath>
#include <rdge/util/fp.hpp>

using namespace RDGE::Util;

TEST(RDGE_UtilTest, FloatingPoint)
{
    EXPECT_TRUE(fp_eq(0.0, 0.0000001));
    EXPECT_TRUE(fp_eq(NAN, NAN));
    EXPECT_TRUE(fp_eq(INFINITY, -INFINITY));

    EXPECT_FALSE(fp_eq(0.0f, 0.000001f));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
