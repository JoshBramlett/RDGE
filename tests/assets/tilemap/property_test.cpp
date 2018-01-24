#include <gtest/gtest.h>

#include <rdge/assets/tilemap/property.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/util/json.hpp>

#include <exception>
#include <fstream>

namespace {

using namespace rdge;
using json = nlohmann::json;

// NOTE: No test for missing elements b/c the json lib has an assertion

TEST(PropertyTest, VerifyConstruction)
{
    std::ifstream stream("../tests/testdata/assets/tilemap/property_01.json");
    json j;
    stream >> j;
    stream.close();

    // 1) Validate proper construction
    tilemap::PropertyCollection properties(j);
    EXPECT_EQ(properties.Size(), 6);

    // 2) Validate accessors
    EXPECT_EQ(properties.GetBool("cust_prop_bool"), true);
    EXPECT_EQ(properties.GetColor("cust_prop_color"), color::from_argb("#ffec9cc6"));
    EXPECT_FLOAT_EQ(properties.GetFloat("cust_prop_float"), 3.14f);
    EXPECT_EQ(properties.GetInt("cust_prop_int"), 5);
    EXPECT_EQ(properties.GetString("cust_prop_string"), "asdf");

    // NOTE will throw if failed
    auto rwops = properties.GetFile("cust_prop_file", "rt");
}

TEST(PropertyTest, VerifyEmptyConstruction)
{
    json j = json::array();
    tilemap::PropertyCollection properties(j);
    EXPECT_EQ(properties.Size(), 0);
}

TEST(PropertyTest, HandleInvalidConstruction)
{
    auto j = R"(
      {
        "properties": [
          {
            "type":"mint",
            "name":"cust_prop_int",
            "value":5
          }
        ]
      }
    )"_json;

    EXPECT_THROW(tilemap::PropertyCollection { j }, std::runtime_error);

    j = R"(
      {
        "properties": [
          {
            "type":"int",
            "name":"cust_prop_int",
            "value":true
          }
        ]
      }
    )"_json;

    EXPECT_THROW(tilemap::PropertyCollection { j }, std::runtime_error);
}

TEST(PropertyTest, HandleInvalidKey)
{
    json j = json::array();
    tilemap::PropertyCollection properties(j);
    EXPECT_EQ(properties.Size(), 0);
    EXPECT_THROW(properties.GetString("bad"), std::runtime_error);
}

TEST(PropertyTest, HandleTypeMismatch)
{
    auto j = R"(
      {
        "properties": [
          {
            "type":"int",
            "name":"cust_prop_int",
            "value":5
          }
        ]
      }
    )"_json;

    tilemap::PropertyCollection properties(j);
    EXPECT_EQ(properties.Size(), 1);
    EXPECT_EQ(properties.GetInt("cust_prop_int"), 5);
    EXPECT_THROW(properties.GetString("cust_prop_int"), std::runtime_error);
}

} // anonymous namespace
