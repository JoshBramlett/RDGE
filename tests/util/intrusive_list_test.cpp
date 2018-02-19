#include <gtest/gtest.h>

#include <rdge/core.hpp>
#include <rdge/util/containers/intrusive_list.hpp>

#include <exception>

namespace {

using namespace rdge;

struct test_node : public intrusive_list_element<test_node>
{
    test_node (uint32 n = 0) : value(n) { }
    uint32 value = 0;
};

TEST(IntrusiveListTest, ValidateMove)
{
    intrusive_list<test_node> list_a;
    test_node a;
    test_node b;
    test_node c;

    list_a.push_front(a);
    list_a.push_front(b);
    list_a.push_front(c);

    // a) validate move ctor
    intrusive_list<test_node> list_b(std::move(list_a));
    EXPECT_FALSE(list_b.empty());
    EXPECT_EQ(list_b.size(), 3);
    EXPECT_EQ(&list_b.front(), &c);
    EXPECT_EQ(&list_b.back(), &a);

    // b) validate move assignment
    intrusive_list<test_node> list_c = std::move(list_b);
    EXPECT_FALSE(list_c.empty());
    EXPECT_EQ(list_c.size(), 3);
    EXPECT_EQ(&list_c.front(), &c);
    EXPECT_EQ(&list_c.back(), &a);

    // c) validate moved from
    EXPECT_TRUE(list_a.empty());
    EXPECT_EQ(list_a.size(), 0);
    EXPECT_TRUE(list_b.empty());
    EXPECT_EQ(list_b.size(), 0);

    // d) validate members
    EXPECT_EQ(c.next, &b);
    EXPECT_EQ(b.prev, &c);
    EXPECT_EQ(b.next, &a);
    EXPECT_EQ(a.prev, &b);
}

TEST(IntrusiveListTest, ValidateClear)
{
    intrusive_list<test_node> list;
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
    EXPECT_EQ(a.prev, nullptr);
    EXPECT_EQ(a.next, nullptr);
    EXPECT_EQ(b.prev, nullptr);
    EXPECT_EQ(b.next, nullptr);
    EXPECT_EQ(c.prev, nullptr);
    EXPECT_EQ(c.next, nullptr);
}

TEST(IntrusiveListTest, ValidatePushFront)
{
    intrusive_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;

    list.push_front(a);
    list.push_front(b);
    list.push_front(c);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(&list.front(), &c);
    EXPECT_EQ(&list.back(), &a);
    EXPECT_EQ(c.next, &b);
    EXPECT_EQ(b.prev, &c);
    EXPECT_EQ(b.next, &a);
    EXPECT_EQ(a.prev, &b);
}

TEST(IntrusiveListTest, ValidatePushBack)
{
    intrusive_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;

    list.push_back(a);
    list.push_back(b);
    list.push_back(c);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(&list.back(), &c);
    EXPECT_EQ(a.next, &b);
    EXPECT_EQ(b.prev, &a);
    EXPECT_EQ(b.next, &c);
    EXPECT_EQ(c.prev, &b);
}

TEST(IntrusiveListTest, ValidateInsert)
{
    intrusive_list<test_node> list;
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
    EXPECT_EQ(&list.back(), &a);
    EXPECT_EQ(a.prev, &b);
    EXPECT_EQ(b.prev, &c);
    EXPECT_EQ(b.next, &a);
    EXPECT_EQ(c.next, &b);

    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);

    // b) validate insert back
    list.insert(list.end(), a);
    list.insert(list.end(), b);
    list.insert(list.end(), c);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(&list.back(), &c);
    EXPECT_EQ(a.next, &b);
    EXPECT_EQ(b.prev, &a);
    EXPECT_EQ(b.next, &c);
    EXPECT_EQ(c.prev, &b);

    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);

    // c) validate insert middle
    list.push_back(a);
    list.push_back(c);
    // d) validate iterators
    intrusive_list<test_node>::iterator ci(&c);
    auto bi = list.insert(ci, b);
    EXPECT_EQ(bi, --ci);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(&list.back(), &c);
    EXPECT_EQ(a.next, &b);
    EXPECT_EQ(b.prev, &a);
    EXPECT_EQ(b.next, &c);
    EXPECT_EQ(c.prev, &b);
}

TEST(IntrusiveListTest, ValidateRemove)
{
    intrusive_list<test_node> list;
    test_node a;
    test_node b;
    test_node c;

    // a) remove a single entry
    list.push_back(a);
    list.remove(a);
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(a.prev, nullptr);
    EXPECT_EQ(a.next, nullptr);

    // b) remove a middle entry
    list.push_back(a);
    list.push_back(b);
    list.push_back(c);
    list.remove(b);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(&list.back(), &c);
    EXPECT_EQ(b.prev, nullptr);
    EXPECT_EQ(b.next, nullptr);
    EXPECT_EQ(a.next, &c);
    EXPECT_EQ(c.prev, &a);

    // c) remove the last entry
    list.remove(c);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(&list.front(), &a);
    EXPECT_EQ(&list.back(), &a);
    EXPECT_EQ(c.prev, nullptr);
    EXPECT_EQ(c.next, nullptr);

    // d) remove the first entry
    list.push_back(b);
    list.remove(a);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(&list.front(), &b);
    EXPECT_EQ(&list.back(), &b);
    EXPECT_EQ(a.prev, nullptr);
    EXPECT_EQ(a.next, nullptr);

    // e) remove all entries
    list.remove(b);
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(b.prev, nullptr);
    EXPECT_EQ(b.next, nullptr);
}

TEST(IntrusiveListTest, ValidateContains)
{
    intrusive_list<test_node> list;
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

TEST(IntrusiveListTest, ValidateForEach)
{
    intrusive_list<test_node> list;
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

    // values have been overwritten
    EXPECT_EQ(a.value, 1);
    EXPECT_EQ(b.value, 2);
    EXPECT_EQ(c.value, 3);
}

TEST(IntrusiveListTest, ValidateIterators)
{
    intrusive_list<test_node> list;
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

    std::string reverse_a;
    std::string reverse_b;
    for (auto rit = list.rbegin(); rit != list.rend(); ++rit)
    {
        reverse_a += std::to_string(rit->value);
    }

    for (auto rit = list.crbegin(); rit != list.crend(); ++rit)
    {
        reverse_b += std::to_string(rit->value);
    }

    EXPECT_EQ(reverse_a, "321");
    EXPECT_EQ(reverse_b, "321");
}

TEST(IntrusiveListTest, ValidateSort)
{
    intrusive_list<test_node> list;
    test_node a(1);
    test_node b(2);
    test_node c(3);
    test_node d(4);
    test_node e(5);

    list.push_back(d);
    list.push_back(b);
    list.push_back(c);
    list.push_back(e);
    list.push_back(a);

    std::string unsorted;
    for (const auto& node : list)
    {
        unsorted += std::to_string(node.value);
    }

    list.sort([](const auto& a, const auto& b) { return a.value < b.value; });

    std::string sorted;
    for (const auto& node : list)
    {
        sorted += std::to_string(node.value);
    }

    EXPECT_EQ(unsorted, "42351");
    EXPECT_EQ(sorted, "12345");
}

} // anonymous namespace
