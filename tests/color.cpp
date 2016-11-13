#include <gtest/gtest.h>
#include <gtest/gtest-death-test.h>

#include <rdge/color.hpp>
#include <rdge/util/exception.hpp>

using namespace RDGE;

TEST(ColorTest, HandlesFromRGBException)
{
    // 1) Empty string
    EXPECT_THROW(RDGE::Color::FromRGB(""), std::runtime_error);

    // 2) Invalid length (short)
    EXPECT_THROW(RDGE::Color::FromRGB("#FFFFF"), std::runtime_error);

    // 3) Invalid length (long)
    EXPECT_THROW(RDGE::Color::FromRGB("FFFFFFF"), std::runtime_error);

    // 4) Invalid value
    EXPECT_THROW(RDGE::Color::FromRGB("#FFFGFF"), std::runtime_error);
}

TEST(ColorTest, HandlesFromRGBSuccess)
{
    auto test_a = RDGE::Color::FromRGB("#FFFFFF");
    EXPECT_TRUE(test_a == Color::White());

    auto test_b = RDGE::Color::FromRGB("FFFFFF");
    EXPECT_TRUE(test_b == Color::White());

    auto test_c = RDGE::Color::FromRGB("ff00ff");
    EXPECT_TRUE(test_c == Color::Magenta());

    auto test_d = RDGE::Color::FromRGB("000000");
    EXPECT_TRUE(test_d == Color::Black());
}

TEST(ColorTest, HandlesFromRGBAException)
{
    // 1) Empty string
    EXPECT_THROW(RDGE::Color::FromRGBA(""), std::runtime_error);

    // 2) Invalid length (short)
    EXPECT_THROW(RDGE::Color::FromRGBA("#FFFFFFF"), std::runtime_error);

    // 3) Invalid length (long)
    EXPECT_THROW(RDGE::Color::FromRGBA("FFFFFFFFF"), std::runtime_error);

    // 4) Invalid value
    EXPECT_THROW(RDGE::Color::FromRGBA("#FFFGFFFF"), std::runtime_error);
}

TEST(ColorTest, HandlesFromRGBASuccess)
{
    auto test_a = RDGE::Color::FromRGBA("#FFFFFFFF");
    EXPECT_TRUE(test_a == Color::White());

    auto test_b = RDGE::Color::FromRGBA("FFFFFFFF");
    EXPECT_TRUE(test_b == Color::White());

    auto test_c = RDGE::Color::FromRGBA("ff00ffff");
    EXPECT_TRUE(test_c == Color::Magenta());

    auto test_d = RDGE::Color::FromRGBA("000000FF");
    EXPECT_TRUE(test_d == Color::Black());

    auto test_e = RDGE::Color::FromRGBA("070d1564");
    EXPECT_TRUE(test_e == Color(7, 13, 21, 100));
}

