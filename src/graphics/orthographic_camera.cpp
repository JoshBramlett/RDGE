#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/internal/opengl_wrapper.hpp>
#include <rdge/util/logger.hpp>

#ifdef RDGE_DEBUG
#include <rdge/debug/renderer.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/physics/aabb.hpp>

#include <imgui/imgui.h>
#endif

namespace rdge {

using namespace rdge::math;

OrthographicCamera::OrthographicCamera (void)
{
    auto vp = opengl::GetViewport();
    this->viewport_size = { vp[2], vp[3] };
    DLOG() << "Camera viewport: " << this->viewport_size;

    Update();
}

OrthographicCamera::OrthographicCamera (float width, float height)
    : viewport_size(width, height)
{
    Update();
}

void
OrthographicCamera::Update (void)
{
    this->projection = mat4::orthographic(this->zoom * -(this->viewport_size.w / 2.f),
                                          this->zoom * (this->viewport_size.w / 2.f),
                                          this->zoom * -(this->viewport_size.h / 2.f),
                                          this->zoom * (this->viewport_size.h / 2.f),
                                          this->near,
                                          this->far);
    this->view = mat4::look_at(this->position,                   // camera eye
                               this->position + this->direction, // what to look at
                               this->up);                        // world "up"
    this->combined = this->projection * this->view;
}

void
OrthographicCamera::Translate (const vec2& displacement)
{
    this->position.x += displacement.x;
    this->position.y += displacement.y;
}

void
OrthographicCamera::SetPosition (const vec2& position)
{
    this->position.x = position.x;
    this->position.y = position.y;
}

void
OrthographicCamera::Rotate (float angle)
{
    auto rotation = mat4::rotation(angle, this->direction);
    this->direction.transform(rotation);
    this->up.transform(rotation);
}

#ifdef RDGE_DEBUG
void
OrthographicCamera::Debug_UpdateWidget (bool* p_open)
{
    if (p_open && !*p_open)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(365.f, 275.f), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Camera", p_open))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Location/Orientation");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("position:  %s", rdge::to_string(this->position).c_str());
    ImGui::Text("direction: %s", rdge::to_string(this->direction).c_str());
    ImGui::Text("up:        %s", rdge::to_string(this->up).c_str());
    ImGui::Unindent(15.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Clipping/Zoom");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("near:      %.3f", this->near);
    ImGui::Text("far:       %.3f", this->far);
    ImGui::SliderFloat("", &this->zoom, this->near, this->far, "zoom = %.3f");
    ImGui::SameLine();
    if (ImGui::Button("RESET"))
    {
        this->zoom = DEFAULT_ZOOM;
    }
    ImGui::Unindent(15.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Viewport");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("size:      %s", rdge::to_string(this->viewport_size).c_str());
    ImGui::Checkbox("Show Default", &this->debug_draw_viewport);
    ImGui::Unindent(15.f);

    ImGui::End();
}

void
OrthographicCamera::Debug_Draw (void)
{
    if (this->debug_draw_viewport)
    {
        float half_x = this->viewport_size.x * 0.5f;
        float half_y = this->viewport_size.y * 0.5f;
        debug::DrawWireFrame(physics::aabb({ -half_x, -half_y },
                                           { half_x, half_y }),
                                           color::RED);
    }
}
#endif

} // namespace rdge
