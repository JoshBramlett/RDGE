#include <rdge/debug/widgets/physics_widget.hpp>
#include <rdge/debug/renderer.hpp>
#include <rdge/graphics/layers/tile_layer.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>

#include <imgui/imgui.h>

namespace rdge {
namespace debug {

void
GraphicsWidget::UpdateWidget (void)
{
    using namespace rdge::debug::settings::graphics;

    if (!show_widget)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(210.f, 410.f), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Graphics", &show_widget))
    {
        ImGui::End();
        return;
    }

    if (active_sprite_layers.empty() && active_tile_layers.empty())
    {
        ImGui::Spacing();
        ImGui::Text("No layers registered");
        ImGui::Spacing();

        ImGui::End();
        return;
    }

    {
        ImGui::Spacing();
        ImGui::Checkbox("Hide all Layers", &hide_all_layers);
        ImGui::Spacing();
    }

    // duplicated check boxes - each requires their own ID
    std::string lbl_hide_layer("Hide Layer##");
    std::string lbl_show_sprite_frames("Show Sprite Frames##");
    std::string lbl_show_renderable_area("Show Renderable Area##");

    for (auto l : active_sprite_layers)
    {
        std::string header_title("SpriteLayer: " + l->name);
        std::string id = std::to_string((std::intptr_t)l);
        if (ImGui::CollapsingHeader(header_title.c_str()))
        {
            ImGui::Text("sprites:  %zu", l->m_spriteCount);
            ImGui::Text("capacity: %zu", l->m_spriteCapacity);
            ImGui::Text("textures: %zu", l->textures.size());

            auto& odata = l->debug_overlay;
            ImGui::Text("sprites drawn: %zu", odata.sprites_drawn);
            {
                auto lbl = lbl_hide_layer + id;
                ImGui::Checkbox(lbl.c_str(), &odata.hide_layer);
            }
            {
                auto lbl = lbl_show_sprite_frames + id;
                ImGui::Checkbox(lbl.c_str(), &odata.draw_sprite_frames);
            }
        }
        ImGui::Spacing();
    }

    for (auto l : active_tile_layers)
    {
        std::string header_title("TileLayer: " + l->name);
        std::string id = std::to_string((std::intptr_t)l);
        if (ImGui::CollapsingHeader(header_title.c_str()))
        {
            ImGui::Text("pos:    %s", rdge::to_string(l->m_grid.pos).c_str());
            ImGui::Text("grid:   %s", rdge::to_string(l->m_grid.size).c_str());
            ImGui::Text("chunks: %zu", l->m_chunks.count);

            auto& odata = l->debug_overlay;
            ImGui::Text("chunks drawn: %zu", odata.chunks_drawn);
            {
                auto lbl = lbl_hide_layer + id;
                ImGui::Checkbox(lbl.c_str(), &odata.hide_layer);
            }
            {
                auto lbl = lbl_show_renderable_area + id;
                ImGui::Checkbox(lbl.c_str(), &odata.draw_renderable_area);
            }
        }
        ImGui::Spacing();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

#ifdef RDGE_DEBUG_PROFILING
#endif
#if 0
    // Leaving here in case profiling metrics are added to the graphics module
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


    ImGui::Text("Debug Drawing");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Checkbox("Show Fixtures", &draw_fixtures);
    ImGui::Checkbox("Show Proxy AABBs", &draw_proxy_aabbs);
    ImGui::Checkbox("Show Joints", &draw_joints);
    ImGui::Checkbox("Show Center of Mass", &draw_center_of_mass);
    ImGui::Checkbox("Show BVH Nodes", &draw_bvh_nodes);
    ImGui::Unindent(15.f);
#endif

    ImGui::End();
}

void
GraphicsWidget::OnWidgetCustomRender (void)
{
    using namespace rdge::debug::settings::graphics;

    for (auto l : active_tile_layers)
    {
        if (l->debug_overlay.draw_renderable_area)
        {
            debug::DrawWireFrame(l->m_bounds, colors::tile_region);
        }
    }
}

} // namespace debug
} // namespace rdge
