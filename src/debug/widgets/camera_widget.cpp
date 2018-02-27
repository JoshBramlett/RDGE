#include <rdge/debug/widgets/camera_widget.hpp>
#include <rdge/debug/renderer.hpp>
#include <rdge/physics/aabb.hpp>

#include <imgui/imgui.h>

namespace rdge {
namespace debug {

void
CameraWidget::UpdateWidget (void)
{
    using namespace rdge::debug::settings::camera;

    if (!show_widget)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(365.f, 275.f), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Camera", &show_widget))
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
    ImGui::Checkbox("Show Default", &draw_viewport);
    ImGui::Unindent(15.f);

    ImGui::End();
}

void
CameraWidget::OnWidgetCustomRender (void)
{
    using namespace rdge::debug::settings::camera;

    if (camera && draw_viewport)
    {
        auto pos = camera->position.xy() - (camera->viewport_size * 0.5f);
        debug::DrawWireFrame(physics::aabb(pos,
                                           camera->viewport_size.w,
                                           camera->viewport_size.h),
                                           colors::viewport);
    }
}

} // namespace debug
} // namespace rdge
