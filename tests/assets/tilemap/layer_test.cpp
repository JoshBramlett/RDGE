#include <gtest/gtest.h>

#include <rdge/assets/tilemap/layer.hpp>
#include <rdge/util/json.hpp>

#include <exception>
#include <fstream>

namespace {

using namespace rdge;
using json = nlohmann::json;

// NOTE: No test for missing elements b/c the json lib has an assertion

TEST(LayerTest, VerifyTileLayerConstruction)
{
    auto j = R"(
      {
        "type":"tilelayer",
        "name":"ground",
        "startx":-16,
        "starty":16,
        "width":4,
        "height":4,
        "offsetx":220,
        "offsety":350,
        "opacity":1,
        "visible":true,
        "data":[1, 2, 1, 2, 3, 1, 3, 1, 2, 2, 3, 3, 4, 4, 4, 1]
      }
    )"_json;

    // 1) Validate proper construction
    tilemap::Layer layer(nullptr, j);
    EXPECT_EQ(layer.type, tilemap::LayerType::TILELAYER);
    EXPECT_EQ(rdge::to_string(layer.type), "TILELAYER");
    EXPECT_EQ(layer.name, "ground");
    EXPECT_FLOAT_EQ(layer.offset.x, 220.f);
    EXPECT_FLOAT_EQ(layer.offset.y, 350.f);
    EXPECT_FLOAT_EQ(layer.opacity, 1.f);
    EXPECT_EQ(layer.visible, true);
    EXPECT_EQ(layer.grid_location.x, -16);
    EXPECT_EQ(layer.grid_location.y, 16);
    EXPECT_EQ(layer.grid_size.x, 4);
    EXPECT_EQ(layer.grid_size.y, 4);

    // 2) Validate base object
    // TODO
}

TEST(LayerTest, VerifyObjectLayerConstruction)
{
    auto j = R"(
      {
        "draworder":"topdown",
        "height":0,
        "name":"people",
        "objects":[ ],
        "opacity":1,
        "type":"objectgroup",
        "visible":false,
        "width":0,
        "x":0,
        "y":0
      }
    )"_json;

    // 1) Validate proper construction
    tilemap::Layer layer(nullptr, j);
    EXPECT_EQ(layer.type, tilemap::LayerType::OBJECTGROUP);
    EXPECT_EQ(rdge::to_string(layer.type), "OBJECTGROUP");
    EXPECT_EQ(layer.name, "people");
    EXPECT_EQ(layer.visible, false);
    EXPECT_FLOAT_EQ(layer.opacity, 1.f);

    // 2) Validate base object
    // TODO
}

// TODO
//TEST(LayerTest, VerifyImageLayerConstruction)
//{
//}

//TEST(LayerTest, VerifyGroupLayerConstruction)
//{
//}

} // anonymous namespace
