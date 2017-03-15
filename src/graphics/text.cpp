#include <rdge/graphics/text.hpp>
#include <rdge/graphics/vops.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/internal/opengl_wrapper.hpp>

namespace {

using namespace rdge;

// TODO needs to be moved to appropriate class
math::vec2
to_ndc (const math::uivec2& size)
{
    auto viewport = opengl::GetViewport();
    return { viewport[2] * (size.w / viewport[2]),
             viewport[3] * (size.h / viewport[3]) };
}

} // anonymous namespace

namespace rdge {

Text::Text (std::string           text,
            const math::vec3&     pos,
            std::shared_ptr<Font> font,
            const color&          color,
            Font::RenderMode      mode)
    : m_text(std::move(text))
    , m_color(color)
    , m_renderMode(mode)
    , m_font(std::move(font))
{
    // NOTE Color of text (set via SDL) vs. color of vertex:
    //      Creating the surface with the SDL_ttf library allows the text to be rendered
    //      a specific color, but since we're using shaders we set the texture color to
    //      white and let the shader do the math to get our desired color.  The benefit
    //      is that we don't need to create a new texture any time we want to change
    //      the text color.

    auto surface = m_font->RenderUTF8(m_text, color::WHITE, m_renderMode);
    m_texture = std::make_shared<Texture>(*surface);

    vops::SetPosition(this->vertices, pos, to_ndc({ m_texture->width, m_texture->height }));
    vops::SetDefaultTexCoords(this->vertices);
    vops::SetColor(this->vertices, m_color);
}

void
Text::SetRenderTarget (SpriteBatch& renderer)
{
    renderer.RegisterTexture(m_texture);
    vops::SetTextureUnitID(this->vertices, m_texture->unit_id);
}

void
Text::Draw (SpriteBatch& renderer)
{
    renderer.Submit(this->vertices);
}

void
Text::SetDepth (float depth)
{
    vops::SetDepth(this->vertices, depth);
}

void
Text::SetText (const std::string& text)
{
    m_text = text;
    Rebuild();
}

void
Text::SetColor (const color& color)
{
    m_color = color;
    vops::SetColor(this->vertices, m_color);

    // Shader will handle the color change - no need to rebuild
    //Rebuild();
}

void
Text::Rebuild (void)
{
    auto surface = m_font->RenderUTF8(m_text, color::WHITE, m_renderMode);
    m_texture->Reload(*surface);

    vops::SetSize(this->vertices, to_ndc({ m_texture->width, m_texture->height }));
}

} // namespace rdge
