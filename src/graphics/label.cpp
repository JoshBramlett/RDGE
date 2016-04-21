#include <rdge/graphics/label.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

#include <memory>

namespace RDGE {
namespace Graphics {

using namespace RDGE::Math;

Label::Label (const std::string& text, float x, float y, RDGE::Font font, const RDGE::Color& color)
    : Renderable2D(vec3(x, y, 0), vec2(0, 0))
    , m_font(std::move(font))
{
    // TODO: Set color should be virtual, and label should override so it can create a
    //       new surface.  In fact, all setters should be virtual.

    m_color = color;

    auto surface = m_font.RenderUTF8(text, m_color, RDGE::Font::RenderMode::Solid);
    m_texture = std::make_shared<GLTexture>(surface);

    SetSize(vec2(
                 16.0f * (m_texture->Width() / 960.0f),
                 9.0f * (m_texture->Height() / 540.0f)
                ));
}

void
Label::SetText (const std::string& text)
{
    auto surface = m_font.RenderUTF8(text, m_color, RDGE::Font::RenderMode::Solid);
    m_texture->ResetData(surface);
    SetSize(vec2(
                 16.0f * (m_texture->Width() / 960.0f),
                 9.0f * (m_texture->Height() / 540.0f)
                ));

    //m_texture->Activate();
}

} // namespace Graphics
} // namespace RDGE
