#include <gtest/gtest.h>

#include <rdge/assets/spritesheet.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

#include <SDL.h>

#include <exception>

#include "../dummy_window.hpp"

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
    EXPECT_EQ(sheet.regions.size(), 2);

    // 1) frame value validation
    const auto& part1 = sheet["uv_1"];
    EXPECT_FLOAT_EQ(part1.size.w, 64.f);
    EXPECT_FLOAT_EQ(part1.size.h, 64.f);

    // origin not set (defaults to center)
    EXPECT_FLOAT_EQ(part1.origin.x, 32.f);
    EXPECT_FLOAT_EQ(part1.origin.y, 32.f);

    // origin set
    const auto& part2 = sheet["uv_2"];
    EXPECT_FLOAT_EQ(part2.origin.x, 16.f);
    EXPECT_FLOAT_EQ(part2.origin.y, 32.f);

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

// TODO Add tests for:
// - further exception handling
// - animations
// - objects

} // anonymous namespace
