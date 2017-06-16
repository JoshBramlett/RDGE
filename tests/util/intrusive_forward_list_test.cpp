#include <gtest/gtest.h>

#include <rdge/core.hpp>
#include <rdge/util/containers/intrusive_list.hpp>

#include <exception>

namespace {

using namespace rdge;

struct test_node : public intrusive_forward_list_element<test_node>
{
    uint32 value = 0;
};

TEST(IntrusiveForwardListTest, ValidatePushFront)
{
    intrusive_forward_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;

    list.push_front(&a);
    list.push_front(&b);
    list.push_front(&c);

    EXPECT_TRUE(list.count == 3);
    EXPECT_TRUE(list.first == &c);
    EXPECT_TRUE(c.next == &b);
    EXPECT_TRUE(b.next == &a);
    EXPECT_TRUE(a.next == nullptr);
}

TEST(IntrusiveForwardListTest, ValidatePushBack)
{
    intrusive_forward_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;

    list.push_back(&a);
    list.push_back(&b);
    list.push_back(&c);

    EXPECT_TRUE(list.count == 3);
    EXPECT_TRUE(list.first == &a);
    EXPECT_TRUE(a.next == &b);
    EXPECT_TRUE(b.next == &c);
    EXPECT_TRUE(c.next == nullptr);
}

TEST(IntrusiveForwardListTest, ValidateRemove)
{
    intrusive_forward_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;

    // a) remove a single entry
    list.push_back(&a);
    list.remove(&a);
    EXPECT_TRUE(list.count == 0);
    EXPECT_TRUE(list.first == nullptr);
    EXPECT_TRUE(a.next == nullptr);

    // b) remove a middle entry
    list.push_back(&a);
    list.push_back(&b);
    list.push_back(&c);
    list.remove(&b);

    EXPECT_TRUE(list.count == 2);
    EXPECT_TRUE(b.next == nullptr);
    EXPECT_TRUE(a.next == &c);

    // c) remove the last entry
    list.remove(&c);

    EXPECT_TRUE(list.count == 1);
    EXPECT_TRUE(c.next == nullptr);
    EXPECT_TRUE(a.next == nullptr);

    // d) remove the first entry
    list.push_back(&b);
    list.remove(&a);

    EXPECT_TRUE(list.count == 1);
    EXPECT_TRUE(list.first == &b);
    EXPECT_TRUE(b.next == nullptr);
    EXPECT_TRUE(a.next == nullptr);
}

TEST(IntrusiveForwardListTest, ValidateContains)
{
    intrusive_forward_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;
    test_node d;

    list.push_back(&a);
    list.push_back(&b);
    list.push_back(&c);

    EXPECT_TRUE(list.contains(&a));
    EXPECT_TRUE(list.contains(&b));
    EXPECT_TRUE(list.contains(&c));
    EXPECT_FALSE(list.contains(&d));
}

TEST(IntrusiveForwardListTest, ValidateForEach)
{
    intrusive_forward_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;

    list.push_back(&a);
    list.push_back(&b);
    list.push_back(&c);

    uint32 inc = 0;
    list.for_each([&](auto* node) {
        node->value = ++inc;
    });

    EXPECT_EQ(a.value, 1);
    EXPECT_EQ(b.value, 2);
    EXPECT_EQ(c.value, 3);
}

} // anonymous namespace
