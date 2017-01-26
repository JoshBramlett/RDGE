#include "shooting_gallery_scene.hpp"

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/graphics.hpp>

using namespace rdge;
using namespace rdge::math;

namespace {
    constexpr float DEPTH_BACK_WALL       = 0.1f;
    //constexpr float DEPTH_TARGET_ROW_3    = 0.8f;
    constexpr float DEPTH_PARTITION_ROW_3 = 0.3;
    //constexpr float DEPTH_TARGET_ROW_2    = 0.6f;
    constexpr float DEPTH_PARTITION_ROW_2 = 0.5f;
    //constexpr float DEPTH_TARGET_ROW_1    = 0.4f;
    constexpr float DEPTH_PARTITION_ROW_1 = 0.7f;
    constexpr float DEPTH_COUNTER         = 0.8f;
    constexpr float DEPTH_CURTAIN         = 0.9f;

    struct world_space
    {
        float width  = 1920.f;
        float height = 1080.f;
        float left   = -960.f;
        float right  = 960.f;
        float bottom = -540.f;
        float top    = 540.f;
    };
} // anonymous namespace


ShootingGalleryScene::ShootingGalleryScene (void)
{
    world_space world;
    SpriteSheet stall_sheet("res/spritesheet_stall.json", true);

    stall_layer.AddSprite(stall_sheet.CreateSpriteChain("bg_wood.png",
                                                        vec3(-960.f, -140.f, DEPTH_BACK_WALL),
                                                        vec2(1920.f, 0.f)));

    stall_layer.AddSprite(stall_sheet.CreateSprite("cloud1.png",
                                                   vec3(350.f, 240.f, DEPTH_BACK_WALL)));

    stall_layer.AddSprite(stall_sheet.CreateSprite("tree_oak.png",
                                                   vec3(-960.f, -40.f, DEPTH_BACK_WALL)));

    {
        auto part1 = stall_sheet["grass1.png"];
        auto part2 = stall_sheet["grass2.png"];
        auto size1 = static_cast<math::vec2>(part1.size);
        auto size2 = static_cast<math::vec2>(part2.size);

        int32 num = static_cast<int32>(1920.f / size1.w) + 1; // Width is the same for both
        float x = -960.f;
        float y = -380.f;
        for (int32 i = 0; i < num; ++i)
        {
            stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(x, y, DEPTH_PARTITION_ROW_3),
                                                           ((i % 2)==0) ? size1 : size2,
                                                           stall_sheet.texture,
                                                           ((i % 2)==0) ? part1.coords : part2.coords));

            x += size1.w;
        }
    }

    stall_layer.AddSprite(stall_sheet.CreateSprite("tree_pine.png",
                                                   vec3(675.f, -185.f, DEPTH_PARTITION_ROW_3)));

    this->water_back = stall_sheet.CreateSpriteChain("water2.png",
                                                     vec3(-960.f, -525.f, DEPTH_PARTITION_ROW_2),
                                                     vec2(2180.f, 0.f));

    this->water_front = stall_sheet.CreateSpriteChain("water2.png",
                                                      vec3(-1000.f, -585.f, DEPTH_PARTITION_ROW_1),
                                                      vec2(2180.f, 0.f));

    stall_layer.AddSprite(this->water_back);
    stall_layer.AddSprite(this->water_front);

    {
        auto counter = std::make_shared<Texture>("res/counter.png");
        float ratio = 1920.f / static_cast<float>(counter->width);
        math::vec2 size(1920.f, static_cast<float>(counter->height) * ratio);

        stall_layer.AddSprite(std::make_shared<Sprite>(math::vec3(-960.f, -540.f, DEPTH_COUNTER),
                                                       size,
                                                       counter));
    }

    {
        const auto& part = stall_sheet["curtain_top.png"];
        auto size = static_cast<math::vec2>(part.size);

        float x_offset = size.w * .8f;
        float y_offset = 10.f;
        vec3 center = { -(size.w / 2.f), 300.f, DEPTH_CURTAIN };
        vec3 left   = { center.x - x_offset, center.y + y_offset, DEPTH_CURTAIN };
        vec3 left2  = { left.x - x_offset, left.y + y_offset, DEPTH_CURTAIN };
        vec3 right  = { center.x + x_offset, center.y + y_offset, DEPTH_CURTAIN };
        vec3 right2 = { right.x + x_offset, right.y + y_offset, DEPTH_CURTAIN };

        stall_layer.AddSprite(stall_sheet.CreateSprite("curtain_top.png", left2));
        stall_layer.AddSprite(stall_sheet.CreateSprite("curtain_top.png", right2));
        stall_layer.AddSprite(stall_sheet.CreateSprite("curtain_top.png", left));
        stall_layer.AddSprite(stall_sheet.CreateSprite("curtain_top.png", right));
        stall_layer.AddSprite(stall_sheet.CreateSprite("curtain_top.png", center));
    }

    {
        const auto& part = stall_sheet["curtain.png"];
        auto size = static_cast<vec2>(part.size);

        stall_layer.AddSprite(std::make_shared<Sprite>(vec3(-970.f, -430.f, DEPTH_CURTAIN),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
        tex_coords flipped = part.coords;
        stall_layer.AddSprite(std::make_shared<Sprite>(vec3(970.f - size.w, -430.f, DEPTH_CURTAIN),
                                                       size,
                                                       stall_sheet.texture,
                                                       flipped.flip_horizontal()));
    }

    {
        const auto& part = stall_sheet["curtain_rope.png"];
        auto size = static_cast<vec2>(part.size);

        stall_layer.AddSprite(std::make_shared<Sprite>(vec3(-980.f, -35.f, DEPTH_CURTAIN),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
        stall_layer.AddSprite(std::make_shared<Sprite>(vec3(980.f - size.w, -35.f, DEPTH_CURTAIN),
                                                       size,
                                                       stall_sheet.texture,
                                                       part.coords));
    }

    stall_layer.AddSprite(stall_sheet.CreateSpriteChain("curtain_straight.png",
                                                        vec3(-960.f, 380.f, DEPTH_CURTAIN),
                                                        vec2(1920.f, 0.f)));
}

void
ShootingGalleryScene::Initialize (void)
{ }

void
ShootingGalleryScene::Terminate (void)
{ }

void
ShootingGalleryScene::Hibernate (void)
{ }

void
ShootingGalleryScene::Activate (void)
{ }

void
ShootingGalleryScene::OnEvent (const Event&)
{ }

void
ShootingGalleryScene::OnUpdate (uint32 ticks)
{
    static uint32 elapsed = 0;
    elapsed += ticks;

    const float displacement = 70.f;
    const float loop_duration = 5.0f;
    const float scale = 3.14159f * 2.0f / loop_duration;

    float t = fmodf(static_cast<float>(elapsed / 1000.f), loop_duration);
    float t_x_s = std::sinf(t * scale);

    vec3 pos_back  = { t_x_s * -displacement, 0.f, 0.f };
    vec3 pos_front = { t_x_s * displacement, 0.f, 0.f };

    this->water_back->transformation  = mat4::translation(pos_back);
    this->water_front->transformation = mat4::translation(pos_front);
}

void
ShootingGalleryScene::OnRender (void)
{
    // TODO test with nultiple layers
    stall_layer.Draw();
}
