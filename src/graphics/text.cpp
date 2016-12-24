#include <rdge/graphics/text.hpp>
#include <rdge/graphics/vops.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/system/window.hpp>

namespace {

    using namespace rdge;
    using namespace rdge::math;

    // TODO: This is a stop-gap solution.  Text rendering differs from other renderable2ds
    //       in that the size is not known until it's built.  I also don't have the option
    //       of calling SampleSizeUTF8 to get the size and sending to the ctor, as setting
    //       the text will cause the texture to be rebuilt and therefore resetting
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
    vec2 ConvertSizeToCameraSpace (uint32 width, uint32 height)
    {
        auto window = Window::GetCurrentWindow();
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
    auto surface = m_font->RenderUTF8(m_text, m_color, m_renderMode);
    m_texture = std::make_shared<Texture>(*surface);
    auto size = ConvertSizeToCameraSpace(m_texture->width, m_texture->height);

    vops::SetPosition(this->vertices, pos, size);
    vops::SetTexCoords(this->vertices);
    // TODO Does color need to be set?  I think it can just be white
    vops::SetColor(this->vertices, m_color);
}

void
Text::Draw (SpriteBatch& renderer)
{
    renderer.Submit(this->vertices);
}

void
Text::SetRenderTarget (SpriteBatch& renderer)
{
    renderer.RegisterTexture(m_texture);

    vops::SetTextureUnitID(this->vertices, m_texture->unit_id);
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
    // TODO Does color need to be set?  I think it can just be white
    vops::SetColor(this->vertices, color);

    Rebuild();
}

void
Text::Rebuild (void)
{
    auto surface = m_font->RenderUTF8(m_text, m_color, m_renderMode);
    m_texture->Reload(*surface);

    auto size = ConvertSizeToCameraSpace(m_texture->width, m_texture->height);
    vops::UpdateSize(this->vertices, size);
}

} // namespace rdge
