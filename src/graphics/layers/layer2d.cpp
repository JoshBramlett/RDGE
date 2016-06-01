#include <rdge/graphics/layers/layer2d.hpp>
#include <rdge/math/functions.hpp>
#include <rdge/glwindow.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

namespace RDGE {
namespace Graphics {

Layer2D::Layer2D (
                  std::unique_ptr<Shader> shader,
                  RDGE::Math::mat4        projection_matrix,
                  float                   zindex,
                  RDGE::UInt16            num_renderables
                 )
    : Layer(std::move(shader), projection_matrix)
    , m_renderer(num_renderables)
    , m_zIndex(RDGE::Math::clamp(zindex, 0.0f, 1.0f))
{
    // For initializing our sampler2D array
    std::vector<RDGE::Int32> texture_units(Shader::MaxFragmentShaderUnits());
    RDGE::Int32 n = {0};
    std::generate(texture_units.begin(), texture_units.end(), [&n]{ return n++; });

    // Reserve memory for our vector of renderables (does nothing if less than current capacity)
    m_renderables.reserve(num_renderables);

    m_shader->Enable();
    m_shader->SetUniformValue("pr_matrix", m_projectionMatrix);
    m_shader->SetUniformValue("textures", texture_units.size(), texture_units.data());
    m_shader->Disable();
}

Layer2D::~Layer2D (void)
{ }

Layer2D::Layer2D (Layer2D&& rhs) noexcept
    : Layer(std::move(rhs))
    , m_renderer(std::move(rhs.m_renderer))
    , m_renderables(std::move(rhs.m_renderables))
    , m_zIndex(rhs.m_zIndex)
{ }

Layer2D&
Layer2D::operator= (Layer2D&& rhs) noexcept
{
    if (this != &rhs)
    {
        Layer::operator=(std::move(rhs));
        m_renderer = std::move(rhs.m_renderer);
        m_renderables = std::move(rhs.m_renderables);
        m_zIndex = rhs.m_zIndex;
    }

    return *this;
}

RDGE::Math::vec2
Layer2D::ConvertScreenCoordinatesToViewport (RDGE::UInt32 x, RDGE::UInt32 y)
{
    // TODO  !!!  IMPORTANT  !!!
    //
    // This is a stop-gap incomplete solution.  For 3D this is done through ray casting,
    // but this solution works for 2D with the assumption the layer uses an orthographic
    // projection matrix and the view and model matrices are indentity matrices.  Also,
    // the viewport must match the entire window.
    //
    //
    // A somewhat convoluated calculation requres explanation
    //
    // With an orthographic projection, the world coordinates are between [-1, 1], however
    // this is generally scaled to the aspect ratio, so for a 16x9 target you'd have
    // world coordinates be [-16, 16] on the x-axis and [-9, 9] on the y-axis.
    //
    // Each whole number in that range is refered to as a "point".  The calculation "ppp"
    // represents the number of screen pixels per point.
    //
    // The [0,0] element in an orthographic matrix is the inverse of (#points / 2).  So
    // for example [-16, 16] would yield (1 / 16) ==> 0.0625.  [1,1] is the element in
    // the matrix for the y-axis.
    //
    // 1) "ppp" - We mulitply the inverse value from the matrix with the viewport
    //            width/height to get the pixels per point.
    //
    // 2) "points" - The viewport width/height divided by the ppp value will yield the
    //               number of points.
    //
    // 3) "ratio" - The ratio is the viewport width/height divided by the total
    //              drawable width/height.

    // An incomplete solution which addresses letterboxes is to multiply the points
    // by the inverse of the ratio of the viewport to the drawable size.  This does
    // not however work when the viewport is only a subsection of the screen.  The
    // "points" will increase past the viewable range, so the viewport edges will
    // retain the correct points.
    //
    //auto drawable_size = window->DrawableSize();
    //auto ratiox = viewport[2] / drawable_size.w;
    //auto ratioy = viewport[3] / drawable_size.h;
    //pointsx *= (1.f / ratiox);
    //pointsy *= (1.f / ratioy);

    auto window  = RDGE::GLWindow::GetCurrentWindow();
    auto window_size = window->Size();
    auto viewport = OpenGL::GetViewport();

    auto pppx = viewport[2] * m_projectionMatrix.elements[0];
    auto pointsx = viewport[2] / pppx;
    auto wx = pointsx * (1.f - (x / (window_size.w / 2.f)));

    auto pppy = viewport[3] * m_projectionMatrix.elements[5];
    auto pointsy = viewport[3] / pppy;
    auto wy = pointsy * (1.f - (y / (window_size.h / 2.f)));

    return RDGE::Math::vec2((wx * -1.f), wy);
}

void
Layer2D::AddRenderable (std::shared_ptr<Renderable2D> renderable)
{
    auto texture = renderable->Texture();
    if (texture)
    {
        m_renderer.RegisterTexture(texture);
    }

    renderable->SetZIndex(m_zIndex);
    m_renderables.push_back(renderable);
}

void
Layer2D::Render (void)
{
    m_shader->Enable();
    m_renderer.PrepSubmit();

    for (auto renderable : m_renderables)
    {
        renderable->Submit(&m_renderer);
    }

    m_renderer.EndSubmit();
    m_renderer.Flush();
}

} // namespace Graphics
} // namespace RDGE
