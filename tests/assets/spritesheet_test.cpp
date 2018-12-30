#include <gtest/gtest.h>
#include "../dummy_window.hpp"

#include <rdge/assets/spritesheet.hpp>

#include <exception>

namespace {

using namespace rdge;
using namespace rdge::math;

// NOTE: No test for missing elements b/c the json lib has an assertion

class SpriteSheetTest : public ::testing::Test
{
protected:
    SpriteSheetTest (void) = default;
    virtual ~SpriteSheetTest (void) noexcept = default;

    rdge::tests::DummyWindow m_window; // For OpenGL context
};


TEST_F(SpriteSheetTest, HandlesSuccess)
{
    SpriteSheet sheet("../tests/testdata/assets/spritesheet_01.json");
    EXPECT_EQ(sheet.regions.size(), 2u);

    // 1) frame value validation
    const auto& part1 = sheet["uv_1"];
    EXPECT_FLOAT_EQ(part1.size.w, 64.f);
    EXPECT_FLOAT_EQ(part1.size.h, 64.f);

    // origin not set (defaults to center)
    EXPECT_FLOAT_EQ(part1.origin.x, 0.5f);
    EXPECT_FLOAT_EQ(part1.origin.y, 0.5f);

    // origin set
    const auto& part2 = sheet["uv_2"];
    EXPECT_FLOAT_EQ(part2.origin.x, 0.25f);
    EXPECT_FLOAT_EQ(part2.origin.y, 0.125f);

    // 2) Texture coordinates match expected (tests float normalization)
    // 2a) Test lower bound where normalized coordinate == 0.f
    const auto& uv1 = part1.coords;
    EXPECT_FLOAT_EQ(uv1.bottom_left.x, 0.f);
    EXPECT_FLOAT_EQ(uv1.bottom_left.y, 0.f);
    EXPECT_FLOAT_EQ(uv1.bottom_right.x, 0.25f);
    EXPECT_FLOAT_EQ(uv1.bottom_right.y, 0.f);
    EXPECT_FLOAT_EQ(uv1.top_left.x, 0.f);
    EXPECT_FLOAT_EQ(uv1.top_left.y, 0.25f);
    EXPECT_FLOAT_EQ(uv1.top_right.x, 0.25f);
    EXPECT_FLOAT_EQ(uv1.top_right.y, 0.25f);

    // 2b) Test upper bound where normalized coordinate == 1.f
    const auto& uv2 = part2.coords;
    EXPECT_FLOAT_EQ(uv2.bottom_left.x, 0.5f);
    EXPECT_FLOAT_EQ(uv2.bottom_left.y, 0.5f);
    EXPECT_FLOAT_EQ(uv2.bottom_right.x, 1.f);
    EXPECT_FLOAT_EQ(uv2.bottom_right.y, 0.5f);
    EXPECT_FLOAT_EQ(uv2.top_left.x, 0.5f);
    EXPECT_FLOAT_EQ(uv2.top_left.y, 1.f);
    EXPECT_FLOAT_EQ(uv2.top_right.x, 1.f);
    EXPECT_FLOAT_EQ(uv2.top_right.y, 1.f);

    // 3) Verify failed lookups throw
    EXPECT_THROW(sheet["nokey"], std::runtime_error);
}

TEST_F(SpriteSheetTest, HandlesFileDoesNotExist)
{
    EXPECT_THROW(SpriteSheet s("nofile.json"), std::runtime_error);
}

TEST_F(SpriteSheetTest, HandlesImageDoesNotExist)
{
    EXPECT_THROW(SpriteSheet s("../tests/testdata/assets/spritesheet_02.json"),
                 std::runtime_error);
}

TEST_F(SpriteSheetTest, ValidatesSlices)
{
    SpriteSheet sheet("../tests/testdata/assets/spritesheet_03.json");
    EXPECT_EQ(sheet.regions.size(), 1u);

    // 1) frame value validation
    const auto& part1 = sheet["slice_test"];
    EXPECT_FLOAT_EQ(part1.size.w, 128.f);
    EXPECT_FLOAT_EQ(part1.size.h, 128.f);

    // 2) slice validation
    EXPECT_EQ(sheet.slices.size(), 3u);

    {
        const auto& slice = sheet.slices[0];
        EXPECT_EQ(slice.name, "nine_patch");
        EXPECT_EQ(slice.color, rdge::color::RED);
        EXPECT_EQ(slice.bounds, screen_rect(128, 128, 48, 48));
        EXPECT_TRUE(slice.is_nine_patch);
        EXPECT_EQ(slice.center, screen_rect(16, 16, 16, 16));
    }

    {
        const auto& slice = sheet.slices[1];
        EXPECT_EQ(slice.name, "single_slice");
        EXPECT_EQ(slice.color, rdge::color::GREEN);
        EXPECT_EQ(slice.bounds, screen_rect(176, 128, 16, 16));
        EXPECT_FALSE(slice.is_nine_patch);
        EXPECT_EQ(slice.center, screen_rect());
    }

    {
        const auto& slice = sheet.slices[2];
        EXPECT_EQ(slice.name, "slice_with_data");
        EXPECT_EQ(slice.color, rdge::color::BLUE);
        EXPECT_EQ(slice.bounds, screen_rect(176, 144, 16, 16));
        EXPECT_FALSE(slice.is_nine_patch);
        EXPECT_EQ(slice.center, screen_rect());
    }
}

// TODO Add tests for:
// - further exception handling
// - animations
// - objects
// - slice user data

} // anonymous namespace
