//! \headerfile <rdge/physics/bvh.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 05/28/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/util/memory/freelist.hpp>

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

} // namespace physics
} // namespace rdge
