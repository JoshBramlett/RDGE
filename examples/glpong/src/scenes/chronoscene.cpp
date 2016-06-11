#include "chronoscene.hpp"

#include <rdge/application.hpp>
#include <rdge/color.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/graphics/text.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/mat4.hpp>
#include <rdge/util/io.hpp>
#include <rdge/util/logger.hpp>
#include <rdge/graphics/renderable2d.hpp>


namespace glpong {

using namespace RDGE::Assets;
using namespace RDGE::GameObjects;
using namespace RDGE::Graphics;
using namespace RDGE::Math;
using UV = UVCoordinates;

ChronoScene::ChronoScene (RDGE::GLWindow* window)
    : Scene::Scene()
    , m_layer(nullptr)
    , m_spriteSheet(nullptr)
    , m_chrono(nullptr)
    , m_chronoState(ChronoState::Idle)
    , m_chronoDirection(ChronoDirection::Down)
    , m_blinkCounter(0)
{
    // 1)  Create and compile our shader used by our rendering layer
    auto v = RDGE::Util::read_text_file("shaders/basic.vert");
    auto f = RDGE::Util::read_text_file("shaders/basic.frag");
    auto shader = std::make_unique<Shader>(v, f);

    // 2)  Set our projection matrix
    auto aspect_ratio = window->TargetAspectRatio();
    auto ortho = mat4::orthographic(0.0f, aspect_ratio.w, 0.0f, aspect_ratio.h, -1.0f, 1.0f);

    // 3)  Create our rendering layer
    m_layer = std::make_shared<Layer2D>(std::move(shader), ortho, 1.0f, 1);

    // 4)  Create chrono

    auto uv_config = RDGE::Util::read_text_file("textures/chrono.json");
    m_spriteSheet = std::make_shared<SpriteSheet>(uv_config);
    std::cout << "use_count=" << m_spriteSheet.use_count() << std::endl;
    auto ss2 = m_spriteSheet->GetSharedPtr();
    std::cout << "use_count=" << m_spriteSheet.use_count() << std::endl;
    auto uv1 = m_spriteSheet->LookupUV(0);
    auto uv2 = m_spriteSheet->LookupUV(1);
    auto uv3 = m_spriteSheet->LookupUV(2);
    auto uv4 = ss2->LookupUV("UV_STAND_FRONT_1");

    std::cout << uv1 << std::endl << uv2 << std::endl << uv3 << std::endl;

    m_uvs.push_back(uv1);
    m_uvs.push_back(uv2);
    m_uvs.push_back(uv3);

    //m_chrono = std::make_shared<Sprite>(2, 2, .95f, 1.95f, m_spriteSheet, m_uvs[0]);
    m_chrono = std::make_shared<Sprite>(2, 2, .95f, 1.95f, m_spriteSheet, uv4);



/*
    m_spriteSheet = std::make_shared<GLTexture>("textures/chrono.png");
    m_uvs = {
        UV {vec2(0, 1), vec2(0, 0), vec2(8.f/64.f, 0), vec2(8.f/64.f, 1)},
        UV {vec2(8.f/64.f, 1), vec2(8.f/64.f, 0), vec2(16.f/64.f, 0), vec2(16.f/64.f, 1)},
        UV {vec2(16.f/64.f, 1), vec2(16.f/64.f, 0), vec2(24.f/64.f, 0), vec2(24.f/64.f, 1)},
        UV {vec2(24.f/64.f, 1), vec2(24.f/64.f, 0), vec2(32.f/64.f, 0), vec2(32.f/64.f, 1)},
        UV {vec2(32.f/64.f, 1), vec2(32.f/64.f, 0), vec2(40.f/64.f, 0), vec2(40.f/64.f, 1)},
        UV {vec2(40.f/64.f, 1), vec2(40.f/64.f, 0), vec2(48.f/64.f, 0), vec2(48.f/64.f, 1)},
        UV {vec2(48.f/64.f, 1), vec2(48.f/64.f, 0), vec2(56.f/64.f, 0), vec2(56.f/64.f, 1)},
        UV {vec2(40.f/64.f, 1), vec2(40.f/64.f, 0), vec2(32.f/64.f, 0), vec2(32.f/64.f, 1)},
        UV {vec2(48.f/64.f, 1), vec2(48.f/64.f, 0), vec2(40.f/64.f, 0), vec2(40.f/64.f, 1)},
        UV {vec2(56.f/64.f, 1), vec2(56.f/64.f, 0), vec2(48.f/64.f, 0), vec2(48.f/64.f, 1)},
    };
    m_spriteSheetBg = std::make_shared<GLTexture>("textures/lchrono_bg.png");

    m_chrono = std::make_shared<Sprite>(2, 2, .95f, 1.95f, m_spriteSheet, m_uvs[0]);
*/
    // 6)  Add our renderables to the layer, and add the layer to the scene
    m_layer->AddRenderable(m_chrono);
    AddLayer("logo", m_layer);
}

void
ChronoScene::ProcessEventPhase (const SDL_Event& event)
{
    // chrono should be an entity, but he's not

    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_w:
                m_blinkCounter = 0;
                m_chronoDirection = ChronoDirection::Up;

                break;
            case SDLK_a:
                m_blinkCounter = 0;
                m_chronoDirection = ChronoDirection::Left;

                break;
            case SDLK_s:
                m_blinkCounter = 0;
                m_chronoDirection = ChronoDirection::Down;

                break;
            case SDLK_d:
                m_blinkCounter = 0;
                m_chronoDirection = ChronoDirection::Right;

                break;
        }
    }

    Scene::ProcessEventPhase(event);
}

void
ChronoScene::ProcessUpdatePhase (RDGE::UInt32 ticks)
{
    Scene::ProcessUpdatePhase(ticks);

    if (m_chronoState == ChronoState::Idle)
    {
        m_blinkCounter += ticks;

        if (m_blinkCounter >= ANIM_BLINK_DELAY_4)
        {
            m_blinkCounter = 0;
        }

        if (m_blinkCounter >= ANIM_BLINK_DELAY_3)
        {
            m_chrono->SetUV(m_uvs[UV_STAND_FRONT_2]);
        }
        else if (m_blinkCounter >= ANIM_BLINK_DELAY_2)
        {
            m_chrono->SetUV(m_uvs[UV_STAND_FRONT_3]);
        }
        else if (m_blinkCounter >= ANIM_BLINK_DELAY_1)
        {
            m_chrono->SetUV(m_uvs[UV_STAND_FRONT_2]);
        }
        else
        {
            m_chrono->SetUV(m_uvs[UV_STAND_FRONT_1]);
        }
    }
}

} // namespace glpong
