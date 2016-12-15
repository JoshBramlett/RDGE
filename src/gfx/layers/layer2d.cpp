#include <rdge/gfx/layers/layer2d.hpp>
#include <rdge/math/functions.hpp>
#include <rdge/system/window.hpp>
#include <rdge/internal/opengl_wrapper.hpp>
#include <rdge/internal/hints.hpp>

//#include <rdge/math/vec4.hpp>

using namespace rdge;
using namespace rdge::gfx;
using namespace rdge::math;

Layer2D::Layer2D (
                  std::unique_ptr<Shader> shader,
                  rdge::math::mat4        projection_matrix,
                  float                   zindex,
                  rdge::uint16            num_renderables
                 )
    : Layer(std::move(shader), projection_matrix)
    , m_renderer(num_renderables)
    , m_zIndex(rdge::math::clamp(zindex, 0.0f, 1.0f))
{
    // For initializing our sampler2D array
    std::vector<rdge::int32> texture_units(Shader::MaxFragmentShaderUnits());
    rdge::int32 n = 0;
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

void
Layer2D::AddRenderable (std::shared_ptr<Renderable2D> renderable)
{
    if (UNLIKELY(!renderable))
    {
        RDGE_THROW("Submitted renderable cannot be null");
    }

    // TODO This fails for groups that contain textured sprites, as the
    //      group has no texture so the underlying textures are not
    //      registered.  I could overload this method, but I'm inclined
    //      to believe a SpriteBatch refector is in order.  Too many hacks
    //      all around.
    auto texture = renderable->Texture();
    if (texture)
    {
        m_renderer.RegisterTexture(texture);
    }

    renderable->SetZIndex(m_zIndex);
    m_renderables.push_back(renderable);
}

/*
void
Layer2D::AddGroup (std::shared_ptr<RDGE::Controls::Control> group)
{
    group->RegisterTextures(&m_renderer);
    m_renderables.push_back(group);

    // TODO: Remove upon refactor
    m_control = group;
}
*/

void
Layer2D::ProcessEventPhase (rdge::Event&)
{
    /*
    // TODO: Remove upon refactor
    if (event.IsMouseMotionEvent())
    {
        auto mme = event.GetMouseMotionEventArgs();
        auto ndc = mme.CursorLocationInNDC();
        auto clip_coords = RDGE::Math::vec4(ndc.x, ndc.y, -1.0f, 1.0f);
        auto inverse = m_projectionMatrix.inverse();
        RDGE::Math::vec4 ray_eye = inverse * clip_coords;

        mme.SetHomogeneousCursorLocation({ray_eye.x, ray_eye.y});
        m_control->OnMouseMotion(mme);
    }
    else if (event.IsMouseButtonEvent())
    {
        auto mbe = event.GetMouseButtonEventArgs();
        m_control->OnMouseButton(mbe);
    }

    // TODO: remove upon refactor - here for the button
    m_control->HandleEvents(event);
    */
}

void
Layer2D::ProcessUpdatePhase (rdge::uint32)
{

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
