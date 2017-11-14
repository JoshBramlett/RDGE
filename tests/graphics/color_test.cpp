#include <gtest/gtest.h>
#include <gtest/gtest-death-test.h>

#include <rdge/graphics/color.hpp>

#include <exception>

namespace {

using namespace rdge;

TEST(ColorTest, HandlesFromRGBException)
{
    // 1) Empty string
    EXPECT_THROW(color::from_rgb(""), std::runtime_error);

    // 2) Invalid length (short)
    EXPECT_THROW(color::from_rgb("#FFFFF"), std::runtime_error);

    // 3) Invalid length (long)
    EXPECT_THROW(color::from_rgb("FFFFFFF"), std::runtime_error);

    // 4) Invalid value
    EXPECT_THROW(color::from_rgb("#FFFGFF"), std::runtime_error);
}

TEST(ColorTest, VerifyFromRGB)
{
    EXPECT_EQ(color::from_rgb("#FFFFFF"), color::WHITE);
    EXPECT_EQ(color::from_rgb("FFFFFF"), color::WHITE);
    EXPECT_EQ(color::from_rgb("ff00ff"), color::MAGENTA);
    EXPECT_EQ(color::from_rgb("000000"), color::BLACK);
    EXPECT_EQ(color::from_rgb("070d15"), color(7, 13, 21, 255));
    EXPECT_EQ(color::from_rgb("#070d15"), color(7, 13, 21, 255));
}

TEST(ColorTest, HandlesFromRGBAException)
{
    // 1) Empty string
    EXPECT_THROW(color::from_rgba(""), std::runtime_error);

    // 2) Invalid length (short)
    EXPECT_THROW(color::from_rgba("#FFFFFFF"), std::runtime_error);

    // 3) Invalid length (long)
    EXPECT_THROW(color::from_rgba("FFFFFFFFF"), std::runtime_error);

    // 4) Invalid value
    EXPECT_THROW(color::from_rgba("#FFFGFFFF"), std::runtime_error);
}

TEST(ColorTest, VerifyFromRGBA)
{
    EXPECT_EQ(color::from_rgba("#FFFFFFFF"), color::WHITE);
    EXPECT_EQ(color::from_rgba("FFFFFFFF"), color::WHITE);
    EXPECT_EQ(color::from_rgba("ff00ffff"), color::MAGENTA);
    EXPECT_EQ(color::from_rgba("000000FF"), color::BLACK);
    EXPECT_EQ(color::from_rgba("070d1564"), color(7, 13, 21, 100));
    EXPECT_EQ(color::from_rgba("#070d1564"), color(7, 13, 21, 100));

    // also accepts RGB values
    EXPECT_EQ(color::from_rgba("070d15"), color(7, 13, 21, 255));
    EXPECT_EQ(color::from_rgba("#070d15"), color(7, 13, 21, 255));
}

TEST(ColorTest, HandlesFromARGBException)
{
    // 1) Empty string
    EXPECT_THROW(color::from_argb(""), std::runtime_error);

    // 2) Invalid length (short)
    EXPECT_THROW(color::from_argb("#FFFFFFF"), std::runtime_error);

    // 3) Invalid length (long)
    EXPECT_THROW(color::from_argb("FFFFFFFFF"), std::runtime_error);

    // 4) Invalid value
    EXPECT_THROW(color::from_argb("#FFFGFFFF"), std::runtime_error);
}

TEST(ColorTest, VerifyFromARGB)
{
    EXPECT_EQ(color::from_argb("#FFFFFFFF"), color::WHITE);
    EXPECT_EQ(color::from_argb("FFFFFFFF"), color::WHITE);
    EXPECT_EQ(color::from_argb("ffff00ff"), color::MAGENTA);
    EXPECT_EQ(color::from_argb("FF000000"), color::BLACK);
    EXPECT_EQ(color::from_argb("64070d15"), color(7, 13, 21, 100));
    EXPECT_EQ(color::from_argb("#64070d15"), color(7, 13, 21, 100));

    // also accepts RGB values
    EXPECT_EQ(color::from_argb("070d15"), color(7, 13, 21, 255));
    EXPECT_EQ(color::from_argb("#070d15"), color(7, 13, 21, 255));
}

TEST(ColorTest, VerifyToString)
{
    color magenta = color::MAGENTA;
    EXPECT_EQ(magenta.to_rgb(), "#FF00FF");
    EXPECT_EQ(magenta.to_rgba(), "#FF00FFFF");
    EXPECT_EQ(magenta.to_argb(), "#FFFF00FF");
}

} // anonymous namespace
