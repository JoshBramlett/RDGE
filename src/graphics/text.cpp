#include <rdge/graphics/text.hpp>
#include <rdge/graphics/gltexture.hpp>
#include <rdge/glwindow.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

namespace RDGE {
namespace Graphics {

using namespace RDGE::Math;
using namespace RDGE::Assets;

namespace {
    // TODO: This is a stop-gap solution.  Text rendering differs from other renderable2ds
    //       in that the size is not known until it's built.  I also don't have the option
    //       of calling SampleSizeUTF8 to get the size and sending to the ctor, as setting
    //       the text or color will cause the texture to be rebuilt and therefore resetting
    //       the size.
    //
    //       In order to keep this modular, I must query the window to get the aspect ratio
    //       and target width/height to convert the size (in screen coords) to the
    //       projection.
    //
    //       As of this writing the size of the surface created by SDL_ttf is the same
    //       regardless of whether the display supports high DPI.  Therefor the target
    //       width and height are used in the calculation instead of the drawable pixels.
    //
    //       THIS WILL FAIL WHEN MULTIPLE DISPLAYS ARE SUPPORTED
    vec2 ConvertSizeToCameraSpace (RDGE::UInt32 width, RDGE::UInt32 height)
    {
        auto window = RDGE::GLWindow::GetCurrentWindow();
        if (!window)
        {
            return vec2(0.0f, 0.0f);
        }

        auto aspect_ratio = window->TargetAspectRatio();
        return vec2(
                    aspect_ratio.w * (width / static_cast<float>(window->TargetWidth())),
                    aspect_ratio.h * (height / static_cast<float>(window->TargetHeight()))
                   );
    }
}

Text::Text (
            std::string           text,
            float                 x,
            float                 y,
            std::shared_ptr<Font> font,
            const RDGE::Color&    color,
            Font::RenderMode      mode
           )
    : Renderable2D(vec3(x, y, 0), vec2(0, 0))
    , m_text(std::move(text))
    , m_font(std::move(font))
    , m_renderMode(mode)
{
    m_color = color;

    auto surface = m_font->RenderUTF8(m_text, m_color, m_renderMode);
    m_texture = std::make_shared<GLTexture>(surface);
    m_size = ConvertSizeToCameraSpace(m_texture->Width(), m_texture->Height());
}

void
Text::SetText (const std::string& text)
{
    m_text = text;

    Rebuild();
}

void
Text::SetColor (const RDGE::Color& color, bool ignore_alpha)
{
    Renderable2D::SetColor(color, ignore_alpha);

    Rebuild();
}

void
Text::Rebuild (void)
{
    auto surface = m_font->RenderUTF8(m_text, m_color, m_renderMode);
    m_texture->ResetData(surface);
    m_size = ConvertSizeToCameraSpace(m_texture->Width(), m_texture->Height());
}

} // namespace Graphics
} // namespace RDGE
