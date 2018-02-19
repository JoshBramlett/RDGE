#include <rdge/debug/widgets/physics_widget.hpp>
#include <rdge/debug/renderer.hpp>
#include <rdge/physics/collision_graph.hpp>

#include <imgui/imgui.h>

namespace rdge {
namespace debug {

using namespace rdge::physics;

void
PhysicsWidget::UpdateWidget (void)
{
    using namespace rdge::debug::settings::physics;

    if (!show_widget)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(210.f, 410.f), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Physics", &show_widget))
    {
        ImGui::End();
        return;
    }

    if (!graph)
    {
        ImGui::Spacing();
        ImGui::Text("CollisionGraph not registered");
        ImGui::Spacing();

        ImGui::End();
        return;
    }

    ImGui::Text("Graph");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("bodies:   %zu", graph->m_bodies.size());
    ImGui::Text("contacts: %zu", graph->m_contacts.size());
    ImGui::Text("joints:   %zu", graph->m_joints.size());
    ImGui::Unindent(15.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("SmallBlockAllocator");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("claimed:         %llu b", graph->block_allocator.usage.claimed);
    ImGui::Text("slack:           %llu b", graph->block_allocator.usage.slack);
    ImGui::Text("large_allocs:    %zu", graph->block_allocator.usage.large_allocs);
    ImGui::Unindent(15.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("BVH Tree");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("height:          %d", graph->m_tree.Height());
    ImGui::Text("nodes:           %zu", graph->m_tree.Size());
    ImGui::Unindent(15.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Profiling (us)");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("create contacts: %lld", graph->debug_profile.create_contacts);
    ImGui::Text("purge contacts:  %lld", graph->debug_profile.purge_contacts);
    ImGui::Text("solve:           %lld", graph->debug_profile.solve);
    ImGui::Text("synchronize:     %lld", graph->debug_profile.synchronize);
    ImGui::Text("---------------------");
    ImGui::Text("total:           %lld", graph->debug_profile.create_contacts +
                                         graph->debug_profile.purge_contacts +
                                         graph->debug_profile.solve +
                                         graph->debug_profile.synchronize);
    ImGui::Unindent(15.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    bool prevent_sleep = graph->IsSleepPrevented();
    ImGui::Text("Properties");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Checkbox("Prevent Sleep", &prevent_sleep);
    ImGui::Unindent(15.f);

    if (prevent_sleep)
    {
        graph->m_flags |= CollisionGraph::PREVENT_SLEEP;
    }
    else
    {
        graph->m_flags &= ~CollisionGraph::PREVENT_SLEEP;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Debug Drawing");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Checkbox("Show Fixtures", &draw_fixtures);
    ImGui::Checkbox("Show Proxy AABBs", &draw_proxy_aabbs);
    ImGui::Checkbox("Show Joints", &draw_joints);
    ImGui::Checkbox("Show Center of Mass", &draw_center_of_mass);
    ImGui::Checkbox("Show BVH Nodes", &draw_bvh_nodes);
    ImGui::Unindent(15.f);

    ImGui::End();
}

void
PhysicsWidget::OnWidgetCustomRender (void)
{
    using namespace rdge::debug::settings::physics;

    if (!graph)
    {
        return;
    }

    if (draw_bvh_nodes)
    {
        graph->m_tree.DebugDraw(scale);
    }

    if (draw_joints)
    {
        graph->m_joints.for_each([=](auto* j) {
            debug::DrawLine(j->body_a->GetPosition(), j->AnchorA(), colors::joints);
            debug::DrawLine(j->body_b->GetPosition(), j->AnchorB(), colors::joints);
        });
    }

    if (draw_fixtures || draw_proxy_aabbs || draw_center_of_mass)
    {
        graph->m_bodies.for_each([=](auto* body) {
            body->fixtures.for_each([=](auto* f) {
                if (draw_fixtures)
                {
                    if (!body->IsSimulating())
                    {
                        debug::DrawWireFrame(f, colors::not_simulating, scale);
                    }
                    else if (!body->IsAwake())
                    {
                        debug::DrawWireFrame(f, colors::sleeping_body, scale);
                    }
                    else
                    {
                        debug::DrawWireFrame(f, f->wireframe, scale);
                    }
                }

                if (draw_proxy_aabbs)
                {
                    debug::DrawWireFrame(f->proxy->box, colors::proxy_aabb, scale);
                }
            });

            if (draw_center_of_mass)
            {
                debug::DrawPoint(body->GetWorldCenter(), colors::center_of_mass, 4.f);
            }
        });
    }
}

} // namespace debug
} // namespace rdge
