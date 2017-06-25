#include <gtest/gtest.h>

#include <rdge/core.hpp>
#include <rdge/util/memory/freelist.hpp>

#include <exception>

namespace {

using namespace rdge;

struct test_object
{
    uint32 a = 0;
    uint32 b = 0;
    uint32 c = 0;
};

TEST(DynamicFreelistTest, ValidateAllocation)
{
    // a) basic initialization
    DynamicFreelist<test_object> a;
    EXPECT_EQ(a.Size(), 0);
    EXPECT_EQ(a.Capacity(), 128);

    // b) explicit initialization
    DynamicFreelist<test_object> b(64);
    EXPECT_EQ(b.Size(), 0);
    EXPECT_EQ(b.Capacity(), 64);
}

TEST(DynamicFreelistTest, ValidateReallocation)
{
    // make a single element list, so reserving twice will realloc
    DynamicFreelist<test_object> a(1);
    EXPECT_EQ(a.Size(), 0);
    EXPECT_EQ(a.Capacity(), 1);

    auto h1 = a.Reserve();

    {
        auto& h1_obj = a[h1];
        h1_obj.a = 3;
        h1_obj.b = 7;
        h1_obj.c = 15;
        EXPECT_EQ(h1, 0);
    }

    EXPECT_EQ(a.Size(), 1);
    EXPECT_EQ(a.Capacity(), 1);

    auto h2 = a.Reserve();
    EXPECT_EQ(h2, 1);

    // a) validate capacity increases by chunk size
    EXPECT_EQ(a.Size(), 2);
    EXPECT_EQ(a.Capacity(), 129);

    // b) validate values persist after reallocation
    {
        const auto& h1_obj = a[h1];
        EXPECT_EQ(h1_obj.a, 3);
        EXPECT_EQ(h1_obj.b, 7);
        EXPECT_EQ(h1_obj.c, 15);
    }
}

TEST(DynamicFreelistTest, ValidateRelease)
{
    DynamicFreelist<test_object> a;

    auto h1 = a.Reserve();
    auto h2 = a.Reserve();
    auto h3 = a.Reserve();
    auto h4 = a.Reserve();
    auto h5 = a.Reserve();

    a.Release(h3);
    EXPECT_EQ(a.Size(), 4);
    EXPECT_TRUE(a.IsReserved(h1));
    EXPECT_TRUE(a.IsReserved(h2));
    EXPECT_FALSE(a.IsReserved(h3));
    EXPECT_TRUE(a.IsReserved(h4));
    EXPECT_TRUE(a.IsReserved(h5));

    a.Release(h1);
    EXPECT_EQ(a.Size(), 3);
    EXPECT_FALSE(a.IsReserved(h1));
    EXPECT_TRUE(a.IsReserved(h2));
    EXPECT_TRUE(a.IsReserved(h4));
    EXPECT_TRUE(a.IsReserved(h5));

    a.Release(h5);
    EXPECT_EQ(a.Size(), 2);
    EXPECT_TRUE(a.IsReserved(h2));
    EXPECT_TRUE(a.IsReserved(h4));
    EXPECT_FALSE(a.IsReserved(h5));

    a.Release(h2);
    EXPECT_EQ(a.Size(), 1);
    EXPECT_FALSE(a.IsReserved(h2));
    EXPECT_TRUE(a.IsReserved(h4));

    a.Release(h4);
    EXPECT_EQ(a.Size(), 0);
    EXPECT_FALSE(a.IsReserved(h4));
}

} // anonymous namespace
