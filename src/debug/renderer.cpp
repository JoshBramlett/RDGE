#include <rdge/debug/renderer.hpp>
#include <rdge/debug/widgets/camera_widget.hpp>
#include <rdge/debug/widgets/physics_widget.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects/iscene.hpp>
#include <rdge/graphics/orthographic_camera.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/math/intrinsics.hpp>
#include <rdge/math/mat4.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/physics/aabb.hpp>
#include <rdge/physics/shapes/ishape.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/physics/shapes/polygon.hpp>
#include <rdge/physics/fixture.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/util/memory/alloc.hpp>

#include <rdge/internal/hints.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

#include <SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_rdge.h>

#include <sstream>

namespace rdge {
namespace debug {

namespace settings {
    // Global overlay
    bool show_overlay = false;

    // Scene menu item
    bool show_camera_widget = false;
    bool show_physics_widget = false;
    bool show_graphics_widget = false;

    // Memory menu item
    bool show_memory_tracker = false;

    // ImGui menu item
    bool show_imgui_test_window = false;

    // Camera Widget
    bool draw_camera_viewport = false;

    // Physics Widget
    bool draw_physics_fixtures = false;
    bool draw_physics_proxy_aabbs = false;
    bool draw_physics_joints = false;
    bool draw_physics_center_of_mass = false;
    bool draw_physics_bvh_nodes = false;
} // namespace settings

using namespace rdge::math;
using namespace rdge::physics;

namespace {

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

        m_shader = Shader(vert.str(), frag.str());

        m_vao = opengl::CreateVertexArray();
        opengl::BindVertexArray(m_vao);

        m_vbo = opengl::CreateBuffer();
        opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);

        uint32 vbo_size = static_cast<uint32>(m_capacity) * POINT_SIZE;
        if (UNLIKELY(!RDGE_MALLOC(m_buffer, vbo_size, nullptr)))
        {
            RDGE_THROW("Failed to allocate memory");
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
        RDGE_FREE(m_buffer, nullptr);
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

    Shader m_shader;
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
        opengl::DrawArrays(GL_LINES, 0, m_submissions);
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

        m_shader = Shader(vert.str(), frag.str());

        m_vao = opengl::CreateVertexArray();
        opengl::BindVertexArray(m_vao);

        m_vbo = opengl::CreateBuffer();
        opengl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);

        uint32 vbo_size = static_cast<uint32>(m_capacity) * LINE_SIZE;
        if (UNLIKELY(!RDGE_MALLOC(m_buffer, vbo_size, nullptr)))
        {
            RDGE_THROW("Failed to allocate memory");
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
        RDGE_FREE(m_buffer, nullptr);
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

    static constexpr uint32 VERTEX_SIZE = sizeof(line_vertex);
    static constexpr uint32 LINE_SIZE = VERTEX_SIZE * 2;

    static constexpr uint32 VATTR_POS_INDEX = 0;
    static constexpr uint32 VATTR_COLOR_INDEX = 1;
    static constexpr const char* UNI_PROJ_MATRIX = "proj_matrix";

    Shader m_shader;
    uint32 m_vao = 0;
    uint32 m_vbo = 0;

    line_vertex* m_buffer = nullptr;
    uint32       m_submissions = 0;
    uint32       m_capacity = 0;
};

class Overlay
{
public:

    bool OnEvent (Event& event)
    {
        ImGui_ImplRDGE_ProcessEvent(&event.sdl_event);

        if (event.IsKeyboardEvent())
        {
            const auto& args = event.GetKeyboardEventArgs();
            if (args.PhysicalKey() == ScanCode::F1 && args.IsKeyPressed())
            {
                settings::show_overlay = !settings::show_overlay;
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
            ImGui::Text("Memory Tracker: %s",
                        (debug::MEMORY_TRACKING_ENABLED ? "Enabled" : "Disabled"));
            ImGui::Text("%.3f frames/sec", io.Framerate);
            ImGui::End();

            // Main menu
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("Scene"))
                {
                    ImGui::MenuItem("Camera", nullptr, &settings::show_camera_widget);
                    ImGui::MenuItem("Physics", nullptr, &settings::show_physics_widget);
                    ImGui::MenuItem("Graphics", nullptr, &settings::show_graphics_widget);
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Memory"))
                {
                    ImGui::MenuItem("Tracker", nullptr, &settings::show_memory_tracker,
                                    debug::MEMORY_TRACKING_ENABLED);
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("ImGui"))
                {
                    ImGui::MenuItem("Test Window", nullptr, &settings::show_imgui_test_window);
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            camera_widget.UpdateWidget();
            physics_widget.UpdateWidget();

            for (auto widget : widgets)
            {
                widget->UpdateWidget();
            }

            if (settings::show_imgui_test_window)
            {
                ImGui::ShowTestWindow(&settings::show_imgui_test_window);
            }

            if (settings::show_memory_tracker)
            {
                debug::ShowMemoryTracker(&settings::show_memory_tracker);
            }
        }
    }

    void
    OnRender (void)
    {
        camera_widget.OnWidgetCustomRender();
        physics_widget.OnWidgetCustomRender();

        for (auto widget : widgets)
        {
            widget->OnWidgetCustomRender();
        }

        ImGui::Render();
    }

    CameraWidget camera_widget;
    PhysicsWidget physics_widget;

    std::vector<IWidget*> widgets;
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

bool ProcessOnEvent (Event& event)
{
    return s_overlay.OnEvent(event);
}

void ProcessOnUpdate (SDL_Window* window, const delta_time& dt)
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
RegisterCamera (OrthographicCamera* camera)
{
    s_overlay.camera_widget.camera = camera;
}

void
RegisterPhysics (CollisionGraph* graph, float scale)
{
    s_overlay.physics_widget.graph = graph;
    s_overlay.physics_widget.scale = scale;
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
DrawWireFrame (const SpriteVertices& vertices, const color& c)
{
    DrawLine(vertices[0].pos.xy(), vertices[1].pos.xy(), c);
    DrawLine(vertices[1].pos.xy(), vertices[2].pos.xy(), c);
    DrawLine(vertices[2].pos.xy(), vertices[3].pos.xy(), c);
    DrawLine(vertices[3].pos.xy(), vertices[0].pos.xy(), c);
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

} // namespace debug
} // namespace rdge

#endif
