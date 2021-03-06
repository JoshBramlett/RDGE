#include <rdge/debug/renderer.hpp>
#include <rdge/debug/widgets/camera_widget.hpp>
#include <rdge/debug/widgets/graphics_widget.hpp>
#include <rdge/debug/widgets/memory_widget.hpp>
#include <rdge/debug/widgets/physics_widget.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects/iscene.hpp>
#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/graphics/shaders/shader_program.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/graphics/layers/tile_layer.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/math/mat4.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/physics/shapes/ishape.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/physics/shapes/polygon.hpp>
#include <rdge/physics/fixture.hpp>
#include <rdge/system/window.hpp>
#include <rdge/util/compiler.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/memory/alloc.hpp>

#include <rdge/internal/opengl_wrapper.hpp>

#include <SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_rdge.h>

#include <sstream>

namespace rdge {
namespace debug {

namespace settings {

// global
bool show_overlay = false;
bool show_imgui_test_window = false;

namespace camera {
    bool show_widget = false;
    rdge::OrthographicCamera* active_camera = nullptr;

    bool draw_viewport = false;

    namespace colors {
        rdge::color viewport = rdge::color::RED;
    } // namespace colors
} // namespace camera

namespace graphics {
    bool show_widget;
    std::vector<rdge::SpriteLayer*> active_sprite_layers;
    std::vector<rdge::TileLayer*> active_tile_layers;

    bool hide_all_layers = false;

    namespace colors {
        rdge::color sprites = rdge::color::YELLOW;
        rdge::color tile_region = rdge::color::YELLOW;
    } // namespace colors
} // namespace graphics

namespace physics {
    bool show_widget = false;
    rdge::physics::CollisionGraph* active_graph = nullptr;
    float scale = 1.f;

    bool draw_fixtures = false;
    bool draw_proxy_aabbs = false;
    bool draw_joints = false;
    bool draw_center_of_mass = false;
    bool draw_bvh_nodes = false;

    namespace colors {
        rdge::color not_simulating = rdge::color(127, 127, 76);
        rdge::color static_body    = rdge::color(127, 230, 127);
        rdge::color kinematic_body = rdge::color(127, 127, 230);
        rdge::color sleeping_body  = rdge::color(152, 152, 152);
        rdge::color dynamic_body   = rdge::color(230, 178, 178);
        rdge::color proxy_aabb     = rdge::color(230, 76, 230);
        rdge::color center_of_mass = rdge::color::WHITE;
        rdge::color joints         = rdge::color::CYAN;
    } // namespace colors
} // namespace physics

namespace memory {
    bool show_widget = false;
} // namespace memory

} // namespace settings

using namespace rdge::math;
using namespace rdge::physics;

namespace {

// Used with CacheSettings/LoadSettings
enum scene_overlay_flags
{
    scene_overlay__none                        = 0x0000,
    scene_overlay__show_overlay                = 0x0001,
    scene_overlay__camera_draw_viewport        = 0x0002,
    scene_overlay__graphics_hide_all_layers    = 0x0004,
    scene_overlay__physics_draw_fixtures       = 0x0008,
    scene_overlay__physics_draw_proxy_aabbs    = 0x0010,
    scene_overlay__physics_draw_joints         = 0x0020,
    scene_overlay__physics_draw_center_of_mass = 0x0040,
    scene_overlay__physics_draw_bvh_nodes      = 0x0080
};

class PointRenderer
{
public:
    static PointRenderer& Instance (void)
    {
        // Lazy instantiation required because we need a valid OpenGL context
        static PointRenderer instance(5000);
        return instance;
    }

    void SetProjection (const math::mat4& projection)
    {
        m_shader.Enable();
        m_shader.SetUniformValue(UNI_PROJ_MATRIX, projection);
        m_shader.Disable();
    }

    void DrawPoint (const math::vec2& pos, uint32 color, float size) noexcept
    {
        SDL_assert(m_submissions <= m_capacity);

        point_vertex* cursor = m_buffer + m_submissions;

        cursor->pos = pos;
        cursor->color = color;
        cursor->size = size;
        cursor++;
        m_submissions++;
    }

    void Flush (void)
    {
        if (m_submissions == 0)
        {
            // drawing is stateless - bail early if nothing has been submitted
            return;
        }

        m_shader.Enable();
        opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);

        void* mapped = opengl::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(mapped, m_buffer, POINT_SIZE * m_submissions);

        opengl::ReleaseBufferPointer(GL_ARRAY_BUFFER);
        opengl::UnbindBuffers(GL_ARRAY_BUFFER);

        opengl::BindVertexArray(m_vao);
        opengl::Enable(GL_PROGRAM_POINT_SIZE);
        opengl::DrawArrays(GL_POINTS, 0, m_submissions);
        opengl::Disable(GL_PROGRAM_POINT_SIZE);
        opengl::UnbindVertexArrays();

        m_submissions = 0;
    }

private:
    explicit PointRenderer (uint16 capacity)
        : m_capacity(capacity)
    {
        SDL_assert(capacity != 0);
        SDL_assert(SDL_GL_GetCurrentContext() != nullptr);

        std::ostringstream vert;
        vert << "#version 330 core\n"
             //
             << "layout (location = " << VATTR_POS_INDEX   << ") in vec2 position;\n"
             << "layout (location = " << VATTR_COLOR_INDEX << ") in vec4 color;\n"
             << "layout (location = " << VATTR_SIZE_INDEX << ") in float size;\n"
             //
             << "uniform mat4 " << UNI_PROJ_MATRIX << ";\n"
             //
             << "out vec4 vertex_color;\n"
             //
             << "void main()\n"
             << "{\n"
             << "  vertex_color = color;\n"
             << "  gl_Position = " << UNI_PROJ_MATRIX << " * vec4(position, 0.0f, 1.0f);\n"
             << "  gl_PointSize = size;\n"
             << "}\n";

        std::ostringstream frag;
        frag << "#version 330 core\n"
             //
             << "layout (location = 0) out vec4 color;\n"
             //
             << "in vec4 vertex_color;\n"
             //
             << "void main()\n"
             << "{\n"
             << "  color = vertex_color;\n"
             << "}\n";

        m_shader = ShaderProgram(vert.str(), frag.str());

        m_vao = opengl::CreateVertexArray();
        opengl::BindVertexArray(m_vao);

        m_vbo = opengl::CreateBuffer();
        opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);

        size_t vbo_size = m_capacity * POINT_SIZE;
        m_buffer = (point_vertex*)RDGE_MALLOC(vbo_size, memory_bucket_debug);
        if (RDGE_UNLIKELY(!m_buffer))
        {
            RDGE_THROW("Memory allocation failed");
        }

        opengl::SetBufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_DYNAMIC_DRAW);

        opengl::EnableVertexAttribute(VATTR_POS_INDEX);
        opengl::VertexAttribPointer(VATTR_POS_INDEX,
                                    3,
                                    GL_FLOAT,
                                    false,
                                    VERTEX_SIZE,
                                    reinterpret_cast<void*>(offsetof(point_vertex, pos)));

        opengl::EnableVertexAttribute(VATTR_COLOR_INDEX);
        opengl::VertexAttribPointer(VATTR_COLOR_INDEX,
                                    4,
                                    GL_UNSIGNED_BYTE,
                                    true,
                                    VERTEX_SIZE,
                                    reinterpret_cast<void*>(offsetof(point_vertex, color)));

        opengl::EnableVertexAttribute(VATTR_SIZE_INDEX);
        opengl::VertexAttribPointer(VATTR_SIZE_INDEX,
                                    1,
                                    GL_FLOAT,
                                    false,
                                    VERTEX_SIZE,
                                    reinterpret_cast<void*>(offsetof(point_vertex, size)));

        opengl::UnbindBuffers(GL_ARRAY_BUFFER);

        auto viewport = opengl::GetViewport();
        auto width  = viewport[2] / 2.f;
        auto height = viewport[3] / 2.f;
        auto projection = mat4::orthographic(-width, width, -height, height, 0.f, 100.f);

        m_shader.Enable();
        m_shader.SetUniformValue(UNI_PROJ_MATRIX, projection);
        m_shader.Disable();

        DLOG() << "debug::PointRenderer"
               << " capacity=" << m_capacity
               << " vao[" << m_vao << "]"
               << " vbo[" << m_vbo << "].size=" << vbo_size;
    }

    ~PointRenderer (void) noexcept
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
        RDGE_FREE(m_buffer, memory_bucket_debug);
    }

    PointRenderer (const PointRenderer&) = delete;
    PointRenderer& operator= (const PointRenderer&) = delete;
    PointRenderer (PointRenderer&&) = delete;
    PointRenderer& operator= (PointRenderer&&) = delete;

    struct point_vertex
    {
        math::vec2 pos;
        uint32     color = 0xFFFFFFFF;
        float      size;
    };

    static constexpr uint32 VERTEX_SIZE = sizeof(point_vertex);
    static constexpr uint32 POINT_SIZE = VERTEX_SIZE;

    static constexpr uint32 VATTR_POS_INDEX = 0;
    static constexpr uint32 VATTR_COLOR_INDEX = 1;
    static constexpr uint32 VATTR_SIZE_INDEX = 2;
    static constexpr const char* UNI_PROJ_MATRIX = "proj_matrix";

    ShaderProgram m_shader;
    uint32 m_vao = 0;
    uint32 m_vbo = 0;

    point_vertex* m_buffer = nullptr;
    uint32        m_submissions = 0;
    uint32        m_capacity = 0;
};

class LineRenderer
{
public:
    static LineRenderer& Instance (void)
    {
        // Lazy instantiation required because we need a valid OpenGL context
        static LineRenderer instance(5000);
        return instance;
    }

    void SetProjection (const math::mat4& projection)
    {
        m_shader.Enable();
        m_shader.SetUniformValue(UNI_PROJ_MATRIX, projection);
        m_shader.Disable();
    }

    void DrawLine (const math::vec2& pa, const math::vec2& pb, uint32 color) noexcept
    {
        SDL_assert(m_submissions <= m_capacity);

        line_vertex* cursor = m_buffer + m_submissions;

        cursor->pos = pa;
        cursor->color = color;
        cursor++;

        cursor->pos = pb;
        cursor->color = color;

        m_submissions += 2;
    }

    void Flush (void)
    {
        if (m_submissions == 0)
        {
            // drawing is stateless - bail early if nothing has been submitted
            return;
        }

        m_shader.Enable();
        opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);

        void* mapped = opengl::GetBufferPointer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(mapped, m_buffer, LINE_SIZE * m_submissions);

        opengl::ReleaseBufferPointer(GL_ARRAY_BUFFER);
        opengl::UnbindBuffers(GL_ARRAY_BUFFER);

        opengl::BindVertexArray(m_vao);
        opengl::DrawArrays(GL_LINES, 0, static_cast<uint32>(m_submissions));
        opengl::UnbindVertexArrays();

        m_submissions = 0;
    }

private:
    explicit LineRenderer (uint16 capacity)
        : m_capacity(capacity)
    {
        SDL_assert(capacity != 0);
        SDL_assert(SDL_GL_GetCurrentContext() != nullptr);

        std::ostringstream vert;
        vert << "#version 330 core\n"
             //
             << "layout (location = " << VATTR_POS_INDEX   << ") in vec2 position;\n"
             << "layout (location = " << VATTR_COLOR_INDEX << ") in vec4 color;\n"
             //
             << "uniform mat4 " << UNI_PROJ_MATRIX << ";\n"
             //
             << "out vec4 vertex_color;\n"
             //
             << "void main()\n"
             << "{\n"
             << "  vertex_color = color;\n"
             << "  gl_Position = " << UNI_PROJ_MATRIX << " * vec4(position, 0.0f, 1.0f);\n"
             << "}\n";

        std::ostringstream frag;
        frag << "#version 330 core\n"
             //
             << "layout (location = 0) out vec4 color;\n"
             //
             << "in vec4 vertex_color;\n"
             //
             << "void main()\n"
             << "{\n"
             << "  color = vertex_color;\n"
             << "}\n";

        m_shader = ShaderProgram(vert.str(), frag.str());

        m_vao = opengl::CreateVertexArray();
        opengl::BindVertexArray(m_vao);

        m_vbo = opengl::CreateBuffer();
        opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);

        size_t vbo_size = m_capacity * LINE_SIZE;
        m_buffer = (line_vertex*)RDGE_MALLOC(vbo_size, memory_bucket_debug);
        if (RDGE_UNLIKELY(!m_buffer))
        {
            RDGE_THROW("Memory allocation failed");
        }

        opengl::SetBufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_DYNAMIC_DRAW);

        opengl::EnableVertexAttribute(VATTR_POS_INDEX);
        opengl::VertexAttribPointer(VATTR_POS_INDEX,
                                    3,
                                    GL_FLOAT,
                                    false,
                                    VERTEX_SIZE,
                                    reinterpret_cast<void*>(offsetof(line_vertex, pos)));

        opengl::EnableVertexAttribute(VATTR_COLOR_INDEX);
        opengl::VertexAttribPointer(VATTR_COLOR_INDEX,
                                    4,
                                    GL_UNSIGNED_BYTE,
                                    true,
                                    VERTEX_SIZE,
                                    reinterpret_cast<void*>(offsetof(line_vertex, color)));

        opengl::UnbindBuffers(GL_ARRAY_BUFFER);

        auto viewport = opengl::GetViewport();
        auto width  = viewport[2] / 2.f;
        auto height = viewport[3] / 2.f;
        auto projection = mat4::orthographic(-width, width, -height, height, 0.f, 100.f);

        m_shader.Enable();
        m_shader.SetUniformValue(UNI_PROJ_MATRIX, projection);
        m_shader.Disable();

        DLOG() << "debug::LineRenderer"
               << " capacity=" << m_capacity
               << " vao[" << m_vao << "]"
               << " vbo[" << m_vbo << "].size=" << vbo_size;
    }

    ~LineRenderer (void) noexcept
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
        RDGE_FREE(m_buffer, memory_bucket_debug);
    }

    LineRenderer (const LineRenderer&) = delete;
    LineRenderer& operator= (const LineRenderer&) = delete;
    LineRenderer (LineRenderer&&) = delete;
    LineRenderer& operator= (LineRenderer&&) = delete;

    struct line_vertex
    {
        math::vec2 pos;
        uint32     color = 0xFFFFFFFF;
    };

    static constexpr size_t VERTEX_SIZE = sizeof(line_vertex);
    static constexpr size_t LINE_SIZE = VERTEX_SIZE * 2;

    static constexpr uint32 VATTR_POS_INDEX = 0;
    static constexpr uint32 VATTR_COLOR_INDEX = 1;
    static constexpr const char* UNI_PROJ_MATRIX = "proj_matrix";

    ShaderProgram m_shader;
    uint32 m_vao = 0;
    uint32 m_vbo = 0;

    line_vertex* m_buffer = nullptr;
    size_t       m_submissions = 0;
    size_t       m_capacity = 0;
};

class Overlay
{
public:
    CameraWidget camera_widget;
    GraphicsWidget graphics_widget;
    MemoryWidget memory_widget;
    PhysicsWidget physics_widget;
    std::vector<IWidget*> widgets;

public:
    Overlay (void)
    {
        this->widgets.reserve(4);
        this->widgets.push_back(&camera_widget);
        this->widgets.push_back(&graphics_widget);
        this->widgets.push_back(&memory_widget);
        this->widgets.push_back(&physics_widget);
    }

    bool OnEvent (Event& event)
    {
        ImGui_ImplRDGE_ProcessEvent(&event.sdl_event);

        if (event.IsKeyboardEvent())
        {
            const auto& args = event.GetKeyboardEventArgs();
            if (args.IsKeyPressed() && !args.IsRepeating())
            {
                switch (args.PhysicalKey())
                {
                case ScanCode::F1:
                    settings::show_overlay = !settings::show_overlay;
                    break;
                case ScanCode::F2:
                    settings::camera::show_widget = !settings::camera::show_widget;
                    break;
                case ScanCode::F3:
                    settings::physics::show_widget = !settings::physics::show_widget;
                    break;
                case ScanCode::F4:
                    settings::graphics::show_widget = !settings::graphics::show_widget;
                    break;
                case ScanCode::F5:
                    settings::memory::show_widget = !settings::memory::show_widget;
                    break;
                default:
                    break;
                }
            }
        }

        ImGuiIO& io = ImGui::GetIO();
        if ((io.WantCaptureKeyboard && event.IsKeyboardEvent()) ||
            (io.WantCaptureMouse && event.IsMouseEvent()) ||
            (io.WantTextInput && event.IsTextInputEvent()))
        {
            // Suppress events if ImGui wants them
            return true;
        }

        return false;
    }

    void OnUpdate (SDL_Window* window, const delta_time&)
    {
        ImGui_ImplRDGE_NewFrame(window);

        if (settings::show_overlay)
        {
            // Overlay: Frame rate
            ImGuiIO& io = ImGui::GetIO();
            ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
            ImGui::SetNextWindowSize(ImVec2(static_cast<float>(io.DisplaySize.x),
                                            static_cast<float>(io.DisplaySize.y)));

            const auto overlay_flags = ImGuiWindowFlags_NoTitleBar |
                                       ImGuiWindowFlags_NoInputs |
                                       ImGuiWindowFlags_AlwaysAutoResize |
                                       ImGuiWindowFlags_NoScrollbar;
            ImGui::Begin("Overlay", nullptr, ImVec2(0.f, 0.f), 0.f, overlay_flags);

            ImGui::SetCursorPos(ImVec2(10.f, 30.f));
            ImGui::Text("%.3f frames/sec", io.Framerate);
            ImGui::End();

            // Main menu
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("Scene"))
                {
                    ImGui::MenuItem("Camera", "F2", &settings::camera::show_widget);
                    ImGui::MenuItem("Graphics", "F3", &settings::graphics::show_widget);
                    ImGui::MenuItem("Physics", "F4", &settings::physics::show_widget);
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Memory"))
                {
                    ImGui::MenuItem("Tracker", "F5", &settings::memory::show_widget);
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Misc"))
                {
                    if (ImGui::MenuItem("Take Screenshot"))
                    {
                        Window::SaveScreenshot();
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("ImGui"))
                {
                    ImGui::MenuItem("Test Window", nullptr, &settings::show_imgui_test_window);
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            for (auto widget : widgets)
            {
                widget->UpdateWidget();
            }

            if (settings::show_imgui_test_window)
            {
                ImGui::ShowTestWindow();
            }
        }
    }

    void
    OnRender (void)
    {
        for (auto widget : widgets)
        {
            widget->OnWidgetCustomRender();
        }

        ImGui::Render();
    }
};

Overlay s_overlay;

} // anonymous namespace

#ifdef RDGE_DEBUG

void
InitializeOverlay (void)
{
    LineRenderer::Instance();
    PointRenderer::Instance();
}

bool
ProcessOnEvent (Event& event)
{
    return s_overlay.OnEvent(event);
}

void
ProcessOnUpdate (SDL_Window* window, const delta_time& dt)
{
    s_overlay.OnUpdate(window, dt);
}

void
ProcessOnRender (void)
{
    s_overlay.OnRender();

    LineRenderer::Instance().Flush();
    PointRenderer::Instance().Flush();
}

void
AddWidget (IWidget* widget)
{
    s_overlay.widgets.push_back(widget);
}

void
RemoveWidget (IWidget* widget)
{
    for (auto it = s_overlay.widgets.begin(); it != s_overlay.widgets.end();)
    {
        if (*it == widget)
        {
            s_overlay.widgets.erase(it);
            break;
        }
        else
        {
            ++it;
        }
    }
}

void
RegisterCamera (OrthographicCamera* camera)
{
    settings::camera::active_camera = camera;
}

void
RegisterPhysics (CollisionGraph* graph, float scale)
{
    settings::physics::active_graph = graph;
    settings::physics::scale = scale;
}

void
RegisterGraphics (SpriteLayer* layer)
{
    settings::graphics::active_sprite_layers.push_back(layer);
}

void
RegisterGraphics (TileLayer* layer)
{
    settings::graphics::active_tile_layers.push_back(layer);
}

void
ClearGraphics (void)
{
    settings::graphics::active_sprite_layers.clear();
    settings::graphics::active_tile_layers.clear();
}

void
SetLineWidth (float width)
{
    auto supported = opengl::GetSupportedLineWidth();
    if (width < supported[0] || width > supported[1])
    {
        WLOG() << "SetLineWidth request is outside supported bounds and will be clamped."
               << "  min=" << supported[0]
               << "  max=" << supported[1]
               << "  requested=" << width;

        width = math::clamp(width, supported[0], supported[1]);
    }

    opengl::SetLineWidth(width);
}

void
SetProjection (const math::mat4& projection)
{
    LineRenderer::Instance().SetProjection(projection);
    PointRenderer::Instance().SetProjection(projection);
}

void
DrawPoint (const math::vec2& pos, const color& c, float size)
{
    PointRenderer::Instance().DrawPoint(pos, static_cast<uint32>(c), size);
}

void
DrawLine (const math::vec2& pa, const math::vec2& pb, const color& c)
{
    LineRenderer::Instance().DrawLine(pa, pb, static_cast<uint32>(c));
}

void
DrawWireFrame (const physics::aabb& box, const color& c, float scale)
{
    DrawLine(box.top_left() * scale, box.top_right() * scale, c);
    DrawLine(box.top_right() * scale, box.bottom_right() * scale, c);
    DrawLine(box.bottom_right() * scale, box.bottom_left() * scale, c);
    DrawLine(box.bottom_left() * scale, box.top_left() * scale, c);
}

void
DrawWireFrame (const circle& circle, const color& c, float scale)
{
    uint32 segments = 16;
    float theta = 0.f;
    float inc = math::PI * 2.0f / static_cast<float>(segments);

    vec2 p { std::cosf(theta), std::sinf(theta) };
    p *= circle.radius;
    p += circle.pos;
    for (uint32 i = 0; i <= segments; ++i)
    {
        theta += inc;
        vec2 next_p = { std::cosf(theta), std::sinf(theta) };
        next_p *= circle.radius;
        next_p += circle.pos;

        DrawLine(p * scale, next_p * scale, c);
        p = next_p;
    }
}

void
DrawWireFrame (const polygon& poly, const color& c, float scale)
{
    for (size_t i = 0; i < poly.count; ++i)
    {
        size_t next_i = (i < (poly.count - 1)) ? i + 1 : 0;
        DrawLine(poly.vertices[i] * scale, poly.vertices[next_i] * scale, c);
    }
}

void
DrawWireFrame (const ishape* shape, const color& c, float scale)
{
    switch (shape->type())
    {
    case ShapeType::CIRCLE:
        DrawWireFrame(*static_cast<const circle*>(shape), c, scale);
        break;
    case ShapeType::POLYGON:
        DrawWireFrame(*static_cast<const polygon*>(shape), c, scale);
        break;
    case ShapeType::INVALID:
    default:
        break;
    }
}

void
DrawWireFrame (const Fixture* fixture, const color& c, float scale)
{
    DrawWireFrame(fixture->shape.world, c, scale);
}

uint32
CacheSettings (void)
{
    uint32 result = 0;

    {
        using namespace rdge::debug::settings;
        SET_FLAG(show_overlay, result, scene_overlay__show_overlay);
    }

    {
        using namespace rdge::debug::settings::camera;
        SET_FLAG(draw_viewport, result, scene_overlay__camera_draw_viewport);
    }

    {
        using namespace rdge::debug::settings::graphics;
        SET_FLAG(hide_all_layers, result, scene_overlay__graphics_hide_all_layers);
    }

    {
        using namespace rdge::debug::settings::physics;
        SET_FLAG(draw_fixtures, result, scene_overlay__physics_draw_fixtures);
        SET_FLAG(draw_proxy_aabbs, result, scene_overlay__physics_draw_proxy_aabbs);
        SET_FLAG(draw_joints, result, scene_overlay__physics_draw_joints);
        SET_FLAG(draw_center_of_mass, result, scene_overlay__physics_draw_center_of_mass);
        SET_FLAG(draw_bvh_nodes, result, scene_overlay__physics_draw_bvh_nodes);
    }

    return result;
}

void
LoadSettings (uint32 flags)
{
    {
        using namespace rdge::debug::settings;
        show_overlay = (flags & scene_overlay__show_overlay);
    }

    {
        using namespace rdge::debug::settings::camera;
        draw_viewport = (flags & scene_overlay__camera_draw_viewport);
    }

    {
        using namespace rdge::debug::settings::graphics;
        hide_all_layers = (flags & scene_overlay__graphics_hide_all_layers);
    }

    {
        using namespace rdge::debug::settings::physics;
        draw_fixtures       = (flags & scene_overlay__physics_draw_fixtures);
        draw_proxy_aabbs    = (flags & scene_overlay__physics_draw_proxy_aabbs);
        draw_joints         = (flags & scene_overlay__physics_draw_joints);
        draw_center_of_mass = (flags & scene_overlay__physics_draw_center_of_mass);
        draw_bvh_nodes      = (flags & scene_overlay__physics_draw_bvh_nodes);
    }
}

} // namespace debug
} // namespace rdge

#endif
