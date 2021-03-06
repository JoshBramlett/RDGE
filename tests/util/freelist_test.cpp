#include <gtest/gtest.h>

#include <rdge/core.hpp>
#include <rdge/util/containers/freelist.hpp>

#include <exception>

namespace {

using namespace rdge;

struct test_object
{
    uint32 a = 0;
    uint32 b = 0;
    uint32 c = 0;
};

TEST(FreelistTest, ValidateAllocation)
{
    // a) basic initialization
    freelist<test_object> a;
    EXPECT_EQ(a.size(), 0u);
    EXPECT_EQ(a.capacity(), 0u);

    // b) explicit initialization
    freelist<test_object> b(64);
    EXPECT_EQ(b.size(), 0u);
    EXPECT_EQ(b.capacity(), 64u);
}

TEST(FreelistTest, ValidateReallocation)
{
    // make a single element list, so reserving twice will realloc
    freelist<test_object> a(1);
    EXPECT_EQ(a.size(), 0u);
    EXPECT_EQ(a.capacity(), 1u);

    auto h1 = a.reserve();

    {
        auto& h1_obj = a[h1];
        h1_obj.a = 3;
        h1_obj.b = 7;
        h1_obj.c = 15;
        EXPECT_EQ(h1, 0u);
    }

    EXPECT_EQ(a.size(), 1u);
    EXPECT_EQ(a.capacity(), 1u);

    auto h2 = a.reserve();
    EXPECT_EQ(h2, 1u);

    // a) validate capacity increases by chunk size
    EXPECT_EQ(a.size(), 2u);
    EXPECT_EQ(a.capacity(), 129u);

    // b) validate values persist after reallocation
    {
        const auto& h1_obj = a[h1];
        EXPECT_EQ(h1_obj.a, 3u);
        EXPECT_EQ(h1_obj.b, 7u);
        EXPECT_EQ(h1_obj.c, 15u);
    }
}

TEST(FreelistTest, ValidateRelease)
{
    freelist<test_object> a;

    auto h1 = a.reserve();
    auto h2 = a.reserve();
    auto h3 = a.reserve();
    auto h4 = a.reserve();
    auto h5 = a.reserve();

    a.release(h3);
    EXPECT_EQ(a.size(), 4u);
    EXPECT_TRUE(a.is_reserved(h1));
    EXPECT_TRUE(a.is_reserved(h2));
    EXPECT_FALSE(a.is_reserved(h3));
    EXPECT_TRUE(a.is_reserved(h4));
    EXPECT_TRUE(a.is_reserved(h5));

    a.release(h1);
    EXPECT_EQ(a.size(), 3u);
    EXPECT_FALSE(a.is_reserved(h1));
    EXPECT_TRUE(a.is_reserved(h2));
    EXPECT_TRUE(a.is_reserved(h4));
    EXPECT_TRUE(a.is_reserved(h5));

    a.release(h5);
    EXPECT_EQ(a.size(), 2u);
    EXPECT_TRUE(a.is_reserved(h2));
    EXPECT_TRUE(a.is_reserved(h4));
    EXPECT_FALSE(a.is_reserved(h5));

    a.release(h2);
    EXPECT_EQ(a.size(), 1u);
    EXPECT_FALSE(a.is_reserved(h2));
    EXPECT_TRUE(a.is_reserved(h4));

    a.release(h4);
    EXPECT_EQ(a.size(), 0u);
    EXPECT_FALSE(a.is_reserved(h4));
}

} // anonymous namespace
