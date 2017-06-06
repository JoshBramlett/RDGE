#include <rdge/physics/bvh.hpp>

namespace rdge {
namespace physics {

int32
BVHTree::CreateProxy (const aabb& box, void* user_data)
{
    int32 handle = CreateNode();

    auto& node = m_nodes[handle];
    node.fat_box = box.fatten(FATTEN_AMOUNT);
    node.user_data = user_data;

    InsertLeaf(handle);
    return handle;
}

void
BVHTree::DestroyProxy (int32 handle)
{
    SDL_assert(handle != bvh_node::NULL_NODE);
    SDL_assert(m_nodes[handle].is_leaf());

    RemoveLeaf(handle);
    m_nodes.Release(handle);
}

bool
BVHTree::MoveProxy (int32 handle, const aabb& box, const math::vec2& displacement)
{
    auto& node = m_nodes[handle];
    SDL_assert(node.is_leaf());

    if (node.fat_box.contains(box))
    {
        return false;
    }

    RemoveLeaf(handle);

    node.fat_box = box.fatten(FATTEN_AMOUNT);
    math::vec2 expansion = displacement * DISP_MULTIPLIER;

    if (expansion.x < 0.f)
    {
        node.fat_box.lo.x += expansion.x;
    }
    else
    {
        node.fat_box.hi.x += expansion.x;
    }

    if (expansion.y < 0.f)
    {
        node.fat_box.lo.y += expansion.y;
    }
    else
    {
        node.fat_box.hi.y += expansion.y;
    }

    InsertLeaf(handle);
    return true;
}

int32
BVHTree::CreateNode (void)
{
    int32 handle = m_nodes.Reserve();

    auto& node = m_nodes[handle];
    node.parent = bvh_node::NULL_NODE;
    node.left = bvh_node::NULL_NODE;
    node.right = bvh_node::NULL_NODE;

    return handle;
}

void
BVHTree::InsertLeaf (int32 leaf_handle)
{
    if (m_root == bvh_node::NULL_NODE)
    {
        m_root = leaf_handle;
        return;
    }

    auto& leaf = m_nodes[leaf_handle];

    int32 test_handle = m_root;
    while (!m_nodes[test_handle].is_leaf())
    {
        auto& node = m_nodes[test_handle];
        float node_area = node.fat_box.perimeter();

        aabb combined = aabb::merge(node.fat_box, leaf.fat_box);
        float combined_area = combined.perimeter();

        // Cost of creating a new parent for this node and the new leaf
        float cost = 2.f * combined_area;

        // Minimum cost of pushing the leaf further down the tree
        float inheritance_cost = 2.f * (combined_area - node_area);

        float left_cost = 0.f;
        float right_cost = 0.f;

        // cost of decending into left child
        auto& left_child = m_nodes[node.left];
        if (left_child.is_leaf())
        {
            aabb temp = aabb::merge(left_child.fat_box, leaf.fat_box);
            left_cost = temp.perimeter() + inheritance_cost;
        }
        else
        {
            aabb temp = aabb::merge(left_child.fat_box, leaf.fat_box);
            float old_area = left_child.fat_box.perimeter();
            float new_area = temp.perimeter();
            left_cost = (new_area - old_area) + inheritance_cost;
        }

        // cost of decending into right child
        auto& right_child = m_nodes[node.right];
        if (right_child.is_leaf())
        {
            aabb temp = aabb::merge(right_child.fat_box, leaf.fat_box);
            right_cost = temp.perimeter() + inheritance_cost;
        }
        else
        {
            aabb temp = aabb::merge(right_child.fat_box, leaf.fat_box);
            float old_area = right_child.fat_box.perimeter();
            float new_area = temp.perimeter();
            right_cost = (new_area - old_area) + inheritance_cost;
        }

        // decend according to minimum cost
        if (cost < left_cost && cost < right_cost)
        {
            break;
        }

        test_handle = (left_cost < right_cost) ? node.left : node.right;
    }

    // Create a new parent
    int32 sibling_handle = test_handle;
    auto& sibling = m_nodes[sibling_handle];

    int32 old_parent_handle = sibling.parent;
    int32 new_parent_handle = CreateNode();

    auto& new_parent = m_nodes[new_parent_handle];
    new_parent.parent = old_parent_handle;
    new_parent.fat_box = aabb::merge(leaf.fat_box, sibling.fat_box);
    new_parent.height = sibling.height + 1;

    if (old_parent_handle == bvh_node::NULL_NODE)
    {
        // Sibling was the root
        new_parent.left = sibling_handle;
        new_parent.right = leaf_handle;
        sibling.parent = new_parent_handle;
        leaf.parent = new_parent_handle;
        m_root = new_parent_handle;
    }
    else
    {
        // Sibling was not the root
        if (m_nodes[old_parent_handle].left == sibling_handle)
        {
            m_nodes[old_parent_handle].left = new_parent_handle;
        }
        else
        {
            m_nodes[old_parent_handle].right = new_parent_handle;
        }

        new_parent.left = sibling_handle;
        new_parent.right = leaf_handle;
        sibling.parent = new_parent_handle;
        leaf.parent = new_parent_handle;
    }

    // Walk back up the tree fixing heights and AABBs
    test_handle = leaf.parent;
    while (test_handle != bvh_node::NULL_NODE)
    {
        test_handle = Balance(test_handle);
        auto& test_node = m_nodes[test_handle];

        SDL_assert(test_node.left != bvh_node::NULL_NODE);
        SDL_assert(test_node.right != bvh_node::NULL_NODE);

        auto& left_node = m_nodes[test_node.left];
        auto& right_node = m_nodes[test_node.right];
        test_node.height = 1 + std::max(left_node.height, right_node.height);
        test_node.fat_box = aabb::merge(left_node.fat_box, right_node.fat_box);

        test_handle = test_node.parent;
    }
}

void
BVHTree::RemoveLeaf (int32 leaf_handle)
{
    if (leaf_handle == m_root)
    {
        m_root = bvh_node::NULL_NODE;
        return;
    }

    auto& leaf_node = m_nodes[leaf_handle];

    int32 parent_handle = leaf_node.parent;
    auto& parent_node = m_nodes[parent_handle];

    int32 grandparent_handle = parent_node.parent;
    auto& grandparent_node = m_nodes[grandparent_handle];

    int32 sibling_handle = (parent_node.left == leaf_handle)
        ? parent_node.right
        : parent_node.left;

    if (grandparent_handle != bvh_node::NULL_NODE)
    {
        // destroy parent and connect sibling to grandparent
        if (grandparent_node.left == parent_handle)
        {
            grandparent_node.left = sibling_handle;
        }
        else
        {
            grandparent_node.right = sibling_handle;
        }

        m_nodes[sibling_handle].parent = grandparent_handle;
        m_nodes.Release(parent_handle);

        // Adjust ancestor bounds
        int32 test_handle = grandparent_handle;
        while (test_handle != bvh_node::NULL_NODE)
        {
            test_handle = Balance(test_handle);
            auto& test_node = m_nodes[test_handle];

            SDL_assert(test_node.left != bvh_node::NULL_NODE);
            SDL_assert(test_node.right != bvh_node::NULL_NODE);

            auto& left_node = m_nodes[test_node.left];
            auto& right_node = m_nodes[test_node.right];
            test_node.height = 1 + std::max(left_node.height, right_node.height);
            test_node.fat_box = aabb::merge(left_node.fat_box, right_node.fat_box);

            test_handle = test_node.parent;
        }
    }
    else
    {
        m_root = sibling_handle;
        m_nodes[sibling_handle].parent = bvh_node::NULL_NODE;
        m_nodes.Release(parent_handle);
    }
}

int32
BVHTree::Balance (int32 handle_a)
{
    SDL_assert(handle_a != bvh_node::NULL_NODE);

    auto& node_a = m_nodes[handle_a];
    if (node_a.is_leaf() || node_a.height < 2)
    {
        return handle_a;
    }

    int32 handle_b = node_a.left;
    int32 handle_c = node_a.right;

    auto& node_b = m_nodes[handle_b];
    auto& node_c = m_nodes[handle_c];
    int32 balance = node_c.height - node_b.height;

    // rotate node_c up
    if (balance > 1)
    {
        int32 handle_f = node_c.left;
        int32 handle_g = node_c.right;
        auto& node_f = m_nodes[handle_f];
        auto& node_g = m_nodes[handle_g];

        // swap node_a and node_c
        node_c.left = handle_a;
        node_c.parent = node_a.parent;
        node_a.parent = handle_c;

        // node_a's old parent should point to node_c
        if (node_c.parent != bvh_node::NULL_NODE)
        {
            if (m_nodes[node_c.parent].left == handle_a)
            {
                m_nodes[node_c.parent].left = handle_c;
            }
            else
            {
                SDL_assert(m_nodes[node_c.parent].right == handle_a);
                m_nodes[node_c.parent].right = handle_c;
            }
        }
        else
        {
            m_root = handle_c;
        }

        // rotate
        if (node_f.height > node_g.height)
        {
            node_c.right = handle_f;
            node_a.right = handle_g;
            node_g.parent = handle_a;

            node_a.fat_box = aabb::merge(node_b.fat_box, node_g.fat_box);
            node_c.fat_box = aabb::merge(node_a.fat_box, node_f.fat_box);
            node_a.height = 1 + std::max(node_b.height, node_g.height);
            node_c.height = 1 + std::max(node_a.height, node_f.height);
        }
        else
        {
            node_c.right = handle_g;
            node_a.right = handle_f;
            node_f.parent = handle_a;

            node_a.fat_box = aabb::merge(node_b.fat_box, node_f.fat_box);
            node_c.fat_box = aabb::merge(node_a.fat_box, node_g.fat_box);
            node_a.height = 1 + std::max(node_b.height, node_f.height);
            node_c.height = 1 + std::max(node_a.height, node_g.height);
        }

        return handle_c;
    }

    // rotate node_b up
    if (balance < -1)
    {
        int32 handle_f = node_b.left;
        int32 handle_g = node_b.right;
        auto& node_f = m_nodes[handle_f];
        auto& node_g = m_nodes[handle_g];

        // swap node_a and node_b
        node_b.left = handle_a;
        node_b.parent = node_a.parent;
        node_a.parent = handle_b;

        // node_a's old parent should point to node_b
        if (node_b.parent != bvh_node::NULL_NODE)
        {
            if (m_nodes[node_b.parent].left == handle_a)
            {
                m_nodes[node_b.parent].left = handle_b;
            }
            else
            {
                SDL_assert(m_nodes[node_b.parent].right == handle_a);
                m_nodes[node_b.parent].right = handle_b;
            }
        }
        else
        {
            m_root = handle_b;
        }

        // rotate
        if (node_f.height > node_g.height)
        {
            node_b.right = handle_f;
            node_a.right = handle_g;
            node_g.parent = handle_a;

            node_a.fat_box = aabb::merge(node_c.fat_box, node_g.fat_box);
            node_b.fat_box = aabb::merge(node_a.fat_box, node_f.fat_box);
            node_a.height = 1 + std::max(node_c.height, node_g.height);
            node_b.height = 1 + std::max(node_a.height, node_f.height);
        }
        else
        {
            node_b.right = handle_g;
            node_a.right = handle_f;
            node_f.parent = handle_a;

            node_a.fat_box = aabb::merge(node_c.fat_box, node_f.fat_box);
            node_b.fat_box = aabb::merge(node_a.fat_box, node_g.fat_box);
            node_a.height = 1 + std::max(node_c.height, node_f.height);
            node_b.height = 1 + std::max(node_a.height, node_g.height);
        }

        return handle_b;
    }

    return handle_a;
}

} // namespace physics
} // namespace rdge
