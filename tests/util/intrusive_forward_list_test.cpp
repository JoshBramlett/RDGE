#include <gtest/gtest.h>

#include <rdge/core.hpp>
#include <rdge/util/containers/intrusive_list.hpp>

#include <exception>

namespace {

using namespace rdge;

struct test_node : public intrusive_forward_list_element<test_node>
{
    test_node (uint32 n = 0) : value(n) { }
    uint32 value = 0;
};

TEST(IntrusiveForwardListTest, ValidateMove)
{
    intrusive_forward_list<test_node> list_a;
    test_node a;
    test_node b;
    test_node c;

    list_a.push_front(a);
    list_a.push_front(b);
    list_a.push_front(c);

    // a) validate move ctor
    intrusive_forward_list<test_node> list_b(std::move(list_a));
    EXPECT_FALSE(list_b.empty());
    EXPECT_EQ(list_b.size(), 3);
    EXPECT_EQ(&list_b.front(), &c);

    // b) validate move assignment
    intrusive_forward_list<test_node> list_c = std::move(list_b);
    EXPECT_FALSE(list_c.empty());
    EXPECT_EQ(list_c.size(), 3);
    EXPECT_EQ(&list_c.front(), &c);

    // c) validate moved from
    EXPECT_TRUE(list_a.empty());
    EXPECT_EQ(list_a.size(), 0);
    EXPECT_TRUE(list_b.empty());
    EXPECT_EQ(list_b.size(), 0);

    // d) validate members
    EXPECT_EQ(c.next, &b);
    EXPECT_EQ(b.next, &a);
}

TEST(IntrusiveForwardListTest, ValidateClear)
{
    intrusive_forward_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;

    list.push_front(a);
    list.push_front(b);
    list.push_front(c);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3);

    list.clear();

    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(a.next, nullptr);
    EXPECT_EQ(b.next, nullptr);
    EXPECT_EQ(c.next, nullptr);
}

TEST(IntrusiveForwardListTest, ValidatePushFront)
{
    intrusive_forward_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;

    list.push_front(a);
    list.push_front(b);
    list.push_front(c);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(&list.front(), &c);
    EXPECT_EQ(c.next, &b);
    EXPECT_EQ(b.next, &a);
}

TEST(IntrusiveForwardListTest, ValidatePushBack)
{
    intrusive_forward_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;

    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(a.next, &b);
    EXPECT_EQ(b.next, &c);
}

TEST(IntrusiveForwardListTest, ValidateInsert)
{
    intrusive_forward_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;

    // a) validate insert front
    list.insert(list.begin(), a);
    list.insert(list.begin(), b);
    list.insert(list.begin(), c);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(&list.front(), &c);
    EXPECT_EQ(b.next, &a);
    EXPECT_EQ(c.next, &b);

    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);

    // b) validate insert middle
    list.push_back(a);
    list.push_back(c);
    // c) validate iterators
    intrusive_forward_list<test_node>::iterator ci(&c);
    auto bi = list.insert(ci, b);
    EXPECT_EQ(++bi, ci);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(a.next, &b);
    EXPECT_EQ(b.next, &c);
}

TEST(IntrusiveForwardListTest, ValidateRemove)
{
    intrusive_forward_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;

    // a) remove a single entry
    list.push_back(a);
    list.remove(a);
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(a.next, nullptr);

    // b) remove a middle entry
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);
    list.remove(b);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(b.next, nullptr);
    EXPECT_EQ(a.next, &c);

    // c) remove the last entry
    list.remove(c);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(c.next, nullptr);

    // d) remove the first entry
    list.push_back(b);
    list.remove(a);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(&list.front(), &b);
    EXPECT_EQ(a.next, nullptr);

    // e) remove all entries
    list.remove(b);
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(b.next, nullptr);
}

TEST(IntrusiveForwardListTest, ValidateContains)
{
    intrusive_forward_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;
    test_node d;

    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    EXPECT_TRUE(list.contains(a));
    EXPECT_TRUE(list.contains(b));
    EXPECT_TRUE(list.contains(c));
    EXPECT_FALSE(list.contains(d));
}

TEST(IntrusiveForwardListTest, ValidateForEach)
{
    intrusive_forward_list<test_node> list;
    test_node a(4);
    test_node b(5);
    test_node c(6);

    uint32 sum = 0;
    list.for_each([&](auto* node) {
        sum += node->value;
    });

    // list is empty
    EXPECT_EQ(sum, 0);

    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    uint32 inc = 0;
    list.for_each([&](auto* node) {
        node->value = ++inc;
    });

    EXPECT_EQ(a.value, 1);
    EXPECT_EQ(b.value, 2);
    EXPECT_EQ(c.value, 3);
}

TEST(IntrusiveForwardListTest, ValidateIterators)
{
    intrusive_forward_list<test_node> list;
    test_node a(1);
    test_node b(2);
    test_node c(3);

    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    std::string forward_a;
    std::string forward_b;
    std::string forward_c;
    for (const auto& node : list)
    {
        forward_a += std::to_string(node.value);
    }

    for (auto it = list.begin(); it != list.end(); ++it)
    {
        forward_b += std::to_string(it->value);
    }

    for (auto it = list.cbegin(); it != list.cend(); ++it)
    {
        forward_c += std::to_string(it->value);
    }

    EXPECT_EQ(forward_a, "123");
    EXPECT_EQ(forward_b, "123");
    EXPECT_EQ(forward_c, "123");
}

} // anonymous namespace
