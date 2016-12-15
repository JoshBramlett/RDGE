#include <gtest/gtest.h>
#include <gtest/gtest-death-test.h>

#include <rdge/gfx/color.hpp>
//#include <rdge/util/exception.hpp>

#include <exception>

using namespace rdge;
using namespace rdge::gfx;

TEST(ColorTest, HandlesFromRGBException)
{
    // 1) Empty string
    EXPECT_THROW(color::FromRGB(""), std::runtime_error);

    // 2) Invalid length (short)
    EXPECT_THROW(color::FromRGB("#FFFFF"), std::runtime_error);

    // 3) Invalid length (long)
    EXPECT_THROW(color::FromRGB("FFFFFFF"), std::runtime_error);

    // 4) Invalid value
    EXPECT_THROW(color::FromRGB("#FFFGFF"), std::runtime_error);
}

TEST(ColorTest, HandlesFromRGBSuccess)
{
    auto test_a = color::FromRGB("#FFFFFF");
    EXPECT_TRUE(test_a == color::WHITE());

    auto test_b = color::FromRGB("FFFFFF");
    EXPECT_TRUE(test_b == color::WHITE());

    auto test_c = color::FromRGB("ff00ff");
    EXPECT_TRUE(test_c == color::MAGENTA());

    auto test_d = color::FromRGB("000000");
    EXPECT_TRUE(test_d == color::BLACK());
}

TEST(ColorTest, HandlesFromRGBAException)
{
    // 1) Empty string
    EXPECT_THROW(color::FromRGBA(""), std::runtime_error);

    // 2) Invalid length (short)
    EXPECT_THROW(color::FromRGBA("#FFFFFFF"), std::runtime_error);

    // 3) Invalid length (long)
    EXPECT_THROW(color::FromRGBA("FFFFFFFFF"), std::runtime_error);

    // 4) Invalid value
    EXPECT_THROW(color::FromRGBA("#FFFGFFFF"), std::runtime_error);
}

TEST(ColorTest, HandlesFromRGBASuccess)
{
    auto test_a = color::FromRGBA("#FFFFFFFF");
    EXPECT_TRUE(test_a == color::WHITE());

    auto test_b = color::FromRGBA("FFFFFFFF");
    EXPECT_TRUE(test_b == color::WHITE());

    auto test_c = color::FromRGBA("ff00ffff");
    EXPECT_TRUE(test_c == color::MAGENTA());

    auto test_d = color::FromRGBA("000000FF");
    EXPECT_TRUE(test_d == color::BLACK());

    auto test_e = color::FromRGBA("070d1564");
    EXPECT_TRUE(test_e == color(7, 13, 21, 100));
}
