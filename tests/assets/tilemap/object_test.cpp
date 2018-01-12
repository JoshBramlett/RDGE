#include <gtest/gtest.h>

#include <rdge/assets/tilemap/object.hpp>
#include <rdge/util/json.hpp>

#include <exception>
#include <fstream>

namespace {

using namespace rdge;
using json = nlohmann::json;

// NOTE: No test for missing elements b/c the json lib has an assertion

// TODO
//TEST(ObjectTest, VerifySpriteConstruction)
//{

//}

TEST(ObjectTest, VerifyPointConstruction)
{
    auto j = R"(
      {
        "id":20,
        "obj_type":"point",
        "name":"point_name",
        "type":"point_type",
        "visible":true,
        "x":220,
        "y":350
      }
    )"_json;

    // 1) Validate proper construction
    tilemap::Object object(j);
    EXPECT_EQ(object.type, tilemap::ObjectType::POINT);
    EXPECT_EQ(rdge::to_string(object.type), "POINT");
    EXPECT_EQ(object.id, 20);
    EXPECT_EQ(object.name, "point_name");
    EXPECT_EQ(object.custom_type, "point_type");
    EXPECT_FLOAT_EQ(object.position.x, 220.f);
    EXPECT_FLOAT_EQ(object.position.y, 350.f);
    EXPECT_EQ(object.visible, true);

    // 2) Validate base object
    auto base = object.GetPoint();
    EXPECT_FLOAT_EQ(base.x, 220.f);
    EXPECT_FLOAT_EQ(base.y, 350.f);
}

TEST(ObjectTest, VerifyCircleConstruction)
{
    auto j = R"(
      {
        "id":13,
        "obj_type":"circle",
        "name":"circle_name",
        "type":"circle_type",
        "visible":true,
        "radius":5,
        "x":560,
        "y":808
      }
    )"_json;

    // 1) Validate proper construction
    tilemap::Object object(j);
    EXPECT_EQ(object.type, tilemap::ObjectType::CIRCLE);
    EXPECT_EQ(rdge::to_string(object.type), "CIRCLE");
    EXPECT_EQ(object.id, 13);
    EXPECT_EQ(object.name, "circle_name");
    EXPECT_EQ(object.custom_type, "circle_type");
    EXPECT_FLOAT_EQ(object.position.x, 560.f);
    EXPECT_FLOAT_EQ(object.position.y, 808.f);
    EXPECT_EQ(object.visible, true);

    // 2) Validate base object
    auto base = object.GetCircle();
    EXPECT_FLOAT_EQ(base.pos.x, 560.f);
    EXPECT_FLOAT_EQ(base.pos.y, 808.f);
    EXPECT_FLOAT_EQ(base.radius, 5.f);
}

TEST(ObjectTest, VerifyPolygonConstruction)
{
    auto j = R"(
      {
        "id":15,
        "obj_type": "polygon",
        "name":"polygon_name",
        "type":"polygon_type",
        "coords":[
        {
          "x":0,
          "y":0
        },
        {
          "x":152,
          "y":88
        },
        {
          "x":136,
          "y":-128
        },
        {
          "x":80,
          "y":-280
        },
        {
          "x":16,
          "y":-288
        }],
        "visible":true,
        "x":-176,
        "y":432
      }
    )"_json;

    // 1) Validate proper construction
    tilemap::Object object(j);
    EXPECT_EQ(object.type, tilemap::ObjectType::POLYGON);
    EXPECT_EQ(rdge::to_string(object.type), "POLYGON");
    EXPECT_EQ(object.id, 15);
    EXPECT_EQ(object.name, "polygon_name");
    EXPECT_EQ(object.custom_type, "polygon_type");
    EXPECT_FLOAT_EQ(object.position.x, -176.f);
    EXPECT_FLOAT_EQ(object.position.y, 432.f);
    EXPECT_EQ(object.visible, true);

    // 2) Validate base object
    auto base = object.GetPolygon();
    EXPECT_EQ(base.count, 5);
    //EXPECT_FLOAT_EQ(base.lo.y, 2.f);
    //EXPECT_FLOAT_EQ(base.hi.x, 7.5f);
    //EXPECT_FLOAT_EQ(base.hi.y, 8.f);
}

TEST(ObjectTest, VerifyPolylineConstruction)
{
    auto j = R"(
      {
        "id":16,
        "obj_type":"polyline",
        "name":"polyline_name",
        "type":"polyline_type",
        "coords":[
        {
          "x":0,
          "y":0
        },
        {
          "x":248,
          "y":-32
        },
        {
          "x":376,
          "y":72
        },
        {
          "x":544,
          "y":288
        },
        {
          "x":656,
          "y":120
        },
        {
          "x":512,
          "y":0
        }],
        "visible":true,
        "x":240,
        "y":88
      }
    )"_json;

    // 1) Validate unsupported
    EXPECT_THROW(tilemap::Object object(j), std::runtime_error);
}

TEST(ObjectTest, VerifyTextConstruction)
{
    auto j = R"(
      {
        "id":20,
        "obj_type":"text",
        "name":"text_name",
        "type":"text_type",
        "visible":true,
        "x":220,
        "y":350,
        "text":
        {
          "text":"Hello World",
          "wrap":true
        }
      }
    )"_json;

    // 1) Validate unsupported
    EXPECT_THROW(tilemap::Object object(j), std::runtime_error);
}

TEST(ObjectTest, VerifyObjectProperties)
{
    auto j = R"(
      {
        "id":20,
        "obj_type":"point",
        "name":"point_name",
        "type":"point_type",
        "visible":true,
        "x":220,
        "y":350,
        "properties": [
          {
            "type":"int",
            "name":"cust_prop_int",
            "value":5
          }
        ]
      }
    )"_json;

    // 1) Validate proper construction
    tilemap::Object object(j);
    EXPECT_EQ(object.type, tilemap::ObjectType::POINT);

    // 1) Validate proper construction
    EXPECT_EQ(object.properties.Size(), 1);
    EXPECT_EQ(object.properties.GetInt("cust_prop_int"), 5);
}

} // anonymous namespace
