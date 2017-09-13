#include <rdge/debug/widgets/camera_widget.hpp>
#include <rdge/debug/renderer.hpp>
#include <rdge/physics/aabb.hpp>

#include <imgui/imgui.h>

namespace rdge {
namespace debug {

void
CameraWidget::Update (void)
{
    if (!settings::show_camera_widget)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(365.f, 275.f), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Camera", &settings::show_camera_widget))
    {
        ImGui::End();
        return;
    }

    if (!camera)
    {
        ImGui::Spacing();
        ImGui::Text("Camera not registered");
        ImGui::Spacing();

        ImGui::End();
        return;
    }

    ImGui::Text("Location/Orientation");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("position:  %s", rdge::to_string(camera->position).c_str());
    ImGui::Text("direction: %s", rdge::to_string(camera->direction).c_str());
    ImGui::Text("up:        %s", rdge::to_string(camera->up).c_str());
    ImGui::Unindent(15.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Clipping/Zoom");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("near:      %.3f", camera->near);
    ImGui::Text("far:       %.3f", camera->far);
    ImGui::SliderFloat("", &camera->zoom, camera->near, camera->far, "zoom = %.3f");
    ImGui::SameLine();
    if (ImGui::Button("RESET"))
    {
        camera->zoom = OrthographicCamera::DEFAULT_ZOOM;
    }
    ImGui::Unindent(15.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Viewport");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("size:      %s", rdge::to_string(camera->viewport_size).c_str());
    ImGui::Checkbox("Show Default", &settings::draw_camera_viewport);
    ImGui::Unindent(15.f);

    ImGui::End();
}

void
CameraWidget::OnCustomRender (void)
{
    if (camera && settings::draw_camera_viewport)
    {
        float half_x = camera->viewport_size.x * 0.5f;
        float half_y = camera->viewport_size.y * 0.5f;
        debug::DrawWireFrame(physics::aabb({ -half_x, -half_y },
                                           { half_x, half_y }),
                                           colors.viewport);
    }
}

} // namespace debug
} // namespace rdge
