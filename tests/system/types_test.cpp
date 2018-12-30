#include <gtest/gtest.h>

#include <rdge/system/types.hpp>
#include <rdge/util/json.hpp>

using namespace rdge;

namespace {

TEST(ScreenPointTest, HandlesConstruction)
{
    {
        screen_point test;
        EXPECT_EQ(test.x, 0);
        EXPECT_EQ(test.y, 0);
    }

    {
        screen_point test { 1, 11 };
        EXPECT_EQ(test.x, 1);
        EXPECT_EQ(test.y, 11);
    }

    {
        SDL_Point p { 1, 11 };
        screen_point test(p);
        EXPECT_EQ(test.x, 1);
        EXPECT_EQ(test.y, 11);
    }
}

TEST(ScreenPointTest, HandlesSerialization)
{
    screen_point test { 1, 11 };
    nlohmann::json j = test;
    EXPECT_TRUE(j.is_object());
    EXPECT_EQ(j.count("x"), 1u);
    EXPECT_EQ(j.count("y"), 1u);

    const auto& j_x = j["x"];
    EXPECT_TRUE(j_x.is_number_integer());
    EXPECT_EQ(j_x.get<int32>(), 1);

    const auto& j_y = j["y"];
    EXPECT_TRUE(j_y.is_number_integer());
    EXPECT_EQ(j_y.get<int32>(), 11);
}

TEST(ScreenPointTest, HandlesDeserialization)
{
    nlohmann::json j = {
        { "x", 1 },
        { "y", 11 }
    };

    auto test = j.get<screen_point>();
    EXPECT_EQ(test.x, 1);
    EXPECT_EQ(test.y, 11);
}

TEST(ScreenRectTest, HandlesConstruction)
{
    {
        screen_rect test;
        EXPECT_EQ(test.x, 0);
        EXPECT_EQ(test.y, 0);
        EXPECT_EQ(test.w, 0);
        EXPECT_EQ(test.h, 0);
    }

    {
        screen_rect test { 1, 11, 22, 33 };
        EXPECT_EQ(test.x, 1);
        EXPECT_EQ(test.y, 11);
        EXPECT_EQ(test.w, 22);
        EXPECT_EQ(test.h, 33);
    }

    {
        SDL_Rect r { 1, 11, 22, 33 };
        screen_rect test(r);
        EXPECT_EQ(test.x, 1);
        EXPECT_EQ(test.y, 11);
        EXPECT_EQ(test.w, 22);
        EXPECT_EQ(test.h, 33);
    }
}

TEST(ScreenRectTest, ValidatesEdgeCornerMembers)
{
    screen_rect test { -50, -30, 100, 60 };
    EXPECT_EQ(test.top(), -30);
    EXPECT_EQ(test.left(), -50);
    EXPECT_EQ(test.bottom(), -90);
    EXPECT_EQ(test.right(), 50);

    EXPECT_EQ(test.top_left(), screen_point(-50, -30));
    EXPECT_EQ(test.top_right(), screen_point(50, -30));
    EXPECT_EQ(test.bottom_left(), screen_point(-50, -90));
    EXPECT_EQ(test.bottom_right(), screen_point(50, -90));
}

TEST(ScreenRectTest, HandlesSerialization)
{
    screen_rect test { 1, 11, 22, 33 };
    nlohmann::json j = test;
    EXPECT_TRUE(j.is_object());
    EXPECT_EQ(j.count("x"), 1u);
    EXPECT_EQ(j.count("y"), 1u);
    EXPECT_EQ(j.count("w"), 1u);
    EXPECT_EQ(j.count("h"), 1u);

    const auto& j_x = j["x"];
    EXPECT_TRUE(j_x.is_number_integer());
    EXPECT_EQ(j_x.get<int32>(), 1);

    const auto& j_y = j["y"];
    EXPECT_TRUE(j_y.is_number_integer());
    EXPECT_EQ(j_y.get<int32>(), 11);

    const auto& j_w = j["w"];
    EXPECT_TRUE(j_w.is_number_integer());
    EXPECT_EQ(j_w.get<int32>(), 22);

    const auto& j_h = j["h"];
    EXPECT_TRUE(j_h.is_number_integer());
    EXPECT_EQ(j_h.get<int32>(), 33);
}

TEST(ScreenRectTest, HandlesDeserialization)
{
    nlohmann::json j = {
        { "x", 1 },
        { "y", 11 },
        { "w", 22 },
        { "h", 33 }
    };

    auto test = j.get<screen_rect>();
    EXPECT_EQ(test.x, 1);
    EXPECT_EQ(test.y, 11);
    EXPECT_EQ(test.w, 22);
    EXPECT_EQ(test.h, 33);
}

} // anonymous namespace
