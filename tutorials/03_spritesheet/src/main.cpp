#include <rdge/core.hpp>
#include <rdge/application.hpp>
#include <rdge/assets/spritesheet.hpp>
#include <rdge/events/event.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/text.hpp>
#include <rdge/graphics/texture.hpp>
#include <rdge/graphics/layers/sprite_layer.hpp>
#include <rdge/graphics/vops.hpp>
#include <rdge/system/window.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>

#include <memory>

// *** Resource shoutouts ***
//
// Images provided by kenney.nl
//  - http://kenney.nl/assets/shooting-gallery

using namespace rdge;

int main ()
{
    app_settings settings;
    settings.window_title = "03_spritesheet";
    settings.enable_png   = true;
    settings.enable_fonts = true;

    // 1) Initialize SDL
    Application app(settings);

    // (Optional) Disable unprocessed events
    DisableEvent(EventType::FingerDown);
    DisableEvent(EventType::FingerUp);
    DisableEvent(EventType::FingerMotion);
    DisableEvent(EventType::MultiGesture);

    // 2) Create window
    Window window(settings);

    // 3) Load assets
    SpriteSheet stall_sheet("res/spritesheet_stall.json");
    auto counter = std::make_shared<Texture>("res/counter.png");

    // NOTE: All vertex position placement is based on the pretense that we're rendering to
    //       the default viewport size of 960x540, and outputting to a high res display which
    //       will double the viewport size (drawable size of 1920x1080).

    SpriteLayer stall_layer;
    float SCALE = 2.f; //< Constant multiplier to scale all images
    float DEPTH = 0.f; //< Vertex depth (z-index)

    // 4) Create renderable graphics
    {
        auto part = stall_sheet["bg_wood.png"];
        auto size = static_cast<math::vec2>(part.size) * SCALE;

        // NOTE: This is a contiguous batch of sprites giving the appearance of a single sprite.
        //       We're unable to use OpenGL texture wrapping because that requires we use the
        //       entire texture and modify the tex_coords to define the wrapping.
        //       Another thing to note is the width/height of the sprites cannot be sub-pixel
        //       or you'll get gaps between the sprites.

        int32 num = static_cast<int32>(1920.f / size.w) + 1;
        float x = -960.f;
        float y = -140.f;
        for (int32 i = 0; i < num; ++i)
        {
            stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(x, y, DEPTH),
                                                           size,
                                                           stall_sheet.texture,
                                                           part.coords));

            x += size.w;
        }
    }

    {
        const auto& part = stall_sheet["cloud1.png"];
        auto size = static_cast<math::vec2>(part.size) * SCALE;

        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(350.f, 240.f, DEPTH),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
    }

    {
        const auto& part = stall_sheet["tree_oak.png"];
        auto size = static_cast<math::vec2>(part.size) * SCALE;

        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(-960.f, -40.f, DEPTH),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
    }

    {
        auto part1 = stall_sheet["grass1.png"];
        auto part2 = stall_sheet["grass2.png"];
        auto size1 = static_cast<math::vec2>(part1.size) * SCALE;
        auto size2 = static_cast<math::vec2>(part2.size) * SCALE;

        int32 num = static_cast<int32>(1920.f / size1.w) + 1; // Width is the same for both
        float x = -960.f;
        float y = -380.f;
        for (int32 i = 0; i < num; ++i)
        {
            stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(x, y, DEPTH),
                                                           ((i % 2)==0) ? size1 : size2,
                                                           stall_sheet.texture,
                                                           ((i % 2)==0) ? part1.coords : part2.coords));

            x += size1.w;
        }
    }

    {
        const auto& part = stall_sheet["tree_pine.png"];
        auto size = static_cast<math::vec2>(part.size) * SCALE;

        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(915.f - size.w, -185.f, DEPTH),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
    }

    {
        auto part = stall_sheet["water2.png"];
        auto size = static_cast<math::vec2>(part.size) * SCALE;

        int32 num = static_cast<int32>(1920.f / size.w) + 1;
        float x = -960.f;
        float y = -80.f - size.h;
        for (int32 i = 0; i < num; ++i)
        {
            stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(x, y, DEPTH),
                                                           size,
                                                           stall_sheet.texture,
                                                           part.coords));

            x += size.w;
        }

        num++;
        x = -960.f - (size.w / 2.f);
        y -= 60.f;
        for (int32 i = 0; i < num; ++i)
        {
            stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(x, y, DEPTH),
                                                           size,
                                                           stall_sheet.texture,
                                                           part.coords));

            x += size.w;
        }
    }

    {
        float ratio = 1920.f / static_cast<float>(counter->width);
        math::vec2 size(1920.f, static_cast<float>(counter->height) * ratio);

        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(-960.f, -540.f, DEPTH),
                                                       size,
                                                       counter));
    }

    {
        const auto& part = stall_sheet["curtain_top.png"];
        auto size = static_cast<math::vec2>(part.size) * SCALE;

        float x_offset = size.w * .8f;
        float y_offset = 10.f;
        math::vec2 center = math::vec2(-(size.w / 2.f), 300.f);
        math::vec2 left   = math::vec2(center.x - x_offset, center.y + y_offset);
        math::vec2 left2  = math::vec2(left.x - x_offset, left.y + y_offset);
        math::vec2 right  = math::vec2(center.x + x_offset, center.y + y_offset);
        math::vec2 right2 = math::vec2(right.x + x_offset, right.y + y_offset);

        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(left2, DEPTH),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(right2, DEPTH),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(left, DEPTH),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(right, DEPTH),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(center, DEPTH),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
    }

    {
        const auto& part = stall_sheet["curtain.png"];
        auto size = static_cast<math::vec2>(part.size) * SCALE;

        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(-970.f, -430.f, DEPTH),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
        tex_coords flipped = part.coords;
        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(970.f - size.w, -430.f, DEPTH),
                                                       size,
                                                       stall_sheet.texture,
                                                       flipped.flip_horizontal()));
    }

    {
        const auto& part = stall_sheet["curtain_rope.png"];
        auto size = static_cast<math::vec2>(part.size) * SCALE;

        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(-980.f, -35.f, DEPTH),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(980.f - size.w, -35.f, DEPTH),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
    }

    {
        auto part = stall_sheet["curtain_straight.png"];
        auto size = static_cast<math::vec2>(part.size) * SCALE;

        int32 num = static_cast<int32>(1920.f / size.w) + 1;
        float x = -960.f;
        float y = 540.f - size.h;
        for (int32 i = 0; i < num; ++i)
        {
            stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(x, y, DEPTH),
                                                           size,
                                                           stall_sheet.texture,
                                                           part.coords));

            x += size.w;
        }
    }

    // 5) Create game loop
    bool running = true;
    Event event;
    while (running)
    {
        // 6) Poll for user input
        while (PollEvent(&event))
        {
            if (event.IsQuitEvent())
            {
                running = false;
                break;
            }
            else if (event.IsKeyboardEvent())
            {
                auto e = event.GetKeyboardEventArgs();
                if (e.Key() == KeyCode::Escape)
                {
                    running = false;
                    break;
                }
            }
        }

        // 7) Render to the screen using the layer
        window.Clear();

        stall_layer.Draw();

        window.Present();
    }

    return 0;
}
