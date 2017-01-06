#include <gtest/gtest.h>

#include <rdge/assets/spritesheet.hpp>
#include <rdge/math/functions.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

#include <SDL.h>

#include <exception>

#include "../dummy_window.hpp"

namespace {

using namespace rdge;
using namespace rdge::math;

// NOTE: Cannot test for missing elements b/c the json lib has an assertion

class SpriteSheetTest : public ::testing::Test
{
protected:
    SpriteSheetTest() { }

    virtual ~SpriteSheetTest() { }

    rdge::tests::DummyWindow m_window; // For OpenGL context
};


TEST_F(SpriteSheetTest, HandlesSuccess)
{
    SpriteSheet sheet("../tests/testdata/assets/spritesheet_01.json");

    // 1) Texture coordinates match expected (tests float normalization)

    // 1a) Test lower bound where normalized coordinate == 0.f
    const auto& uv1 = sheet["uv_1"].coords;
    EXPECT_TRUE(fp_eq(uv1.bottom_left.x, 0.f));
    EXPECT_TRUE(fp_eq(uv1.bottom_left.y, 0.f));
    EXPECT_TRUE(fp_eq(uv1.bottom_right.x, 0.25f));
    EXPECT_TRUE(fp_eq(uv1.bottom_right.y, 0.f));
    EXPECT_TRUE(fp_eq(uv1.top_left.x, 0.f));
    EXPECT_TRUE(fp_eq(uv1.top_left.y, 0.25f));
    EXPECT_TRUE(fp_eq(uv1.top_right.x, 0.25f));
    EXPECT_TRUE(fp_eq(uv1.top_right.y, 0.25f));

    // 1b) Test upper bound where normalized coordinate == 1.f
    const auto& uv2 = sheet["uv_2"].coords;
    EXPECT_TRUE(fp_eq(uv2.bottom_left.x, 0.5f));
    EXPECT_TRUE(fp_eq(uv2.bottom_left.y, 0.5f));
    EXPECT_TRUE(fp_eq(uv2.bottom_right.x, 1.f));
    EXPECT_TRUE(fp_eq(uv2.bottom_right.y, 0.5f));
    EXPECT_TRUE(fp_eq(uv2.top_left.x, 0.5f));
    EXPECT_TRUE(fp_eq(uv2.top_left.y, 1.f));
    EXPECT_TRUE(fp_eq(uv2.top_right.x, 1.f));
    EXPECT_TRUE(fp_eq(uv2.top_right.y, 1.f));

    // 2) Check for texture
    EXPECT_TRUE(sheet.texture != nullptr);

    // 3) Check CreateSprite
    auto sprite = sheet.CreateSprite("uv_1", vec3());
    EXPECT_TRUE(sprite != nullptr);

    // 4) Verify failed lookups throw
    EXPECT_THROW(sheet["nokey"], std::runtime_error);
    EXPECT_THROW(sheet.CreateSprite("nokey", vec3()), std::runtime_error);
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

TEST_F(SpriteSheetTest, HandlesMissingArray)
{
    EXPECT_THROW(SpriteSheet s("../tests/testdata/assets/spritesheet_03.json"),
                 std::runtime_error);
}

TEST_F(SpriteSheetTest, HandlesValueNotUnsigned)
{
    EXPECT_THROW(SpriteSheet s("../tests/testdata/assets/spritesheet_04.json"),
                 std::runtime_error);
}

TEST_F(SpriteSheetTest, HandlesValueLargerThanSurfaceSize)
{
    EXPECT_THROW(SpriteSheet s("../tests/testdata/assets/spritesheet_05.json"),
                 std::runtime_error);
}

TEST_F(SpriteSheetTest, HandlesDuplicateKeys)
{
    EXPECT_THROW(SpriteSheet s("../tests/testdata/assets/spritesheet_06.json"),
                 std::runtime_error);
}

} // anonymous namespace
