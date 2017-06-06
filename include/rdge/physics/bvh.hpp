//! \headerfile <rdge/physics/bvh.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/28/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/util/memory/freelist.hpp>

#include <vector>
#include <utility>
#include <algorithm>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

struct bvh_node
{
    static constexpr int32 NULL_NODE = -1;

    aabb fat_box;

    int32 height;
    int32 parent;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#pragma GCC diagnostic ignored "-Wnested-anon-types"

    union
    {
        struct
        {
            int32 left;
            int32 right;
        };

        void* user_data;
    };

#pragma GCC diagnostic pop

    bool is_leaf (void) const noexcept
    {
        return (right == NULL_NODE);
    }
};

//! \class BVHTree
//! \brief Bounding Volume Hierarchy
//! \see http://www.randygaul.net/2013/08/06/dynamic-aabb-tree/
//! \see https://www.codeproject.com/Articles/832957/Dynamic-Bounding-Volume-Hiearchy-in-Csharp
class BVHTree
{
public:
    static constexpr float FATTEN_AMOUNT = 0.1f;  //!< Padding added to aabb

    //! \brief Displacement multiplier for movement predictive AABB expansion
    static constexpr float DISP_MULTIPLIER = 2.f;

    BVHTree (void) = default;
    ~BVHTree (void) noexcept = default;

    // TODO add move/copy

    int32 CreateProxy (const aabb& box, void* user_data);
    void DestroyProxy (int32 handle);
    bool MoveProxy (int32 handle, const aabb& box, const math::vec2& displacement);

    //! \brief Query for all intersecting pairs for the provided handles
    //! \details Resultant list of intersecting pairs is guaranteed to be
    //!          unique and will be sorted based by handle.  List may be
    //!          empty if there are no intersecting pairs.
    //! \param [in] handles List of handles to query for intersections
    //! \returns List of typecasted proxy pairs
    template <typename T>
    std::vector<std::pair<T*, T*>> Query (const std::vector<int32>& handles) const;

    //! \brief Query for all proxies intersecting with the provided aabb
    //! \details Resultant list will not be sorted.  List may be empty if
    //!          there are no intersecting proxies.
    //! \param [in] box aabb to query
    //! \returns List of typecasted proxies
    template <typename T>
    std::vector<T*> Query (const aabb& box) const;

    int32 Height (void) const noexcept
    {
        return (m_root == bvh_node::NULL_NODE) ? 0 : m_nodes[m_root].height;
    }

private:

    int32 CreateNode (void);
    void InsertLeaf (int32 leaf_handle);
    void RemoveLeaf (int32 leaf_handle);
    int32 Balance (int32 handle);

    DynamicFreelist<bvh_node> m_nodes;
    int32 m_root;
};

template <typename T>
inline std::vector<std::pair<T*, T*>>
BVHTree::Query (const std::vector<int32>& handles) const
{
    std::vector<std::pair<int32, int32>> pairs(64);
    std::vector<int32> stack(256);
    for (int32 handle_a : handles)
    {
        auto& node_a = m_nodes[handle_a];
        SDL_assert(node_a.is_leaf());

        stack.push_back(m_root);
        while (!stack.empty())
        {
            int32 handle_b = stack.back();
            stack.pop_back();

            if (handle_b == bvh_node::NULL_NODE || handle_a == handle_b)
            {
                continue;
            }

            auto& node_b = m_nodes[handle_b];
            if (node_a.fat_box.intersects_with(node_b.fat_box))
            {
                if (node_b.is_leaf())
                {
                    if (handle_a < handle_b)
                    {
                        pairs.emplace_back(handle_a, handle_b);
                    }
                    else
                    {
                        pairs.emplace_back(handle_b, handle_a);
                    }
                }
                else
                {
                    stack.push_back(node_b.left);
                    stack.push_back(node_b.right);
                }
            }
        }
    }

    // sort pairs to make duplicates adjacent
    std::sort(pairs.begin(), pairs.end(), [](auto& a, auto& b) {
        if (a.first == b.first)
        {
            return (a.second < b.second);
        }

        return (a.first < b.first);
    });

    // remove adjacent duplicates
    auto last = std::unique(pairs.begin(), pairs.end());
    pairs.erase(last, pairs.end());

    std::vector<std::pair<T*, T*>> result(pairs.size());
    for (auto& p : pairs)
    {
        result.emplace_back(reinterpret_cast<T*>(m_nodes[p.first].user_data),
                            reinterpret_cast<T*>(m_nodes[p.second].user_data));
    }

    return result;
}

template <typename T>
inline std::vector<T*>
BVHTree::Query (const aabb& box) const
{
    std::vector<T*> result(64);
    std::vector<int32> stack(256);

    stack.push_back(m_root);
    while (!stack.empty())
    {
        int32 handle = stack.back();
        stack.pop_back();

        if (handle == bvh_node::NULL_NODE)
        {
            continue;
        }

        auto& node = m_nodes[handle];
        if (box.intersects_with(node.fat_box))
        {
            if (node.is_leaf())
            {
                result.push_back(reinterpret_cast<T*>(node.user_data));
            }
            else
            {
                stack.push_back(node.left);
                stack.push_back(node.right);
            }
        }
    }

    return result;
}

} // namespace physics
} // namespace rdge
