#pragma once

#include <rdge/types.hpp>
#include <rdge/glwindow.hpp>
#include <rdge/gameobjects/scene.hpp>
#include <rdge/graphics/gltexture.hpp>
#include <rdge/graphics/spritesheet.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/layers/layer2d.hpp>

#include <vector>
#include <memory>

namespace glpong {

enum class ChronoState
{
    Idle,
    Walking,
    Running
};

enum class ChronoDirection
{
    Up,
    Right,
    Left,
    Down
};

class ChronoScene final : public RDGE::GameObjects::Scene
{
public:
    explicit ChronoScene (RDGE::GLWindow* window);

    virtual ~ChronoScene (void) { }

    virtual void ProcessEventPhase (const SDL_Event& event) override;

    virtual void ProcessUpdatePhase (RDGE::UInt32 ticks) override;

private:
    static constexpr RDGE::UInt8 UV_STAND_FRONT_1 = 0;
    static constexpr RDGE::UInt8 UV_STAND_FRONT_2 = 1;
    static constexpr RDGE::UInt8 UV_STAND_FRONT_3 = 2;
    static constexpr RDGE::UInt8 UV_STAND_BACK    = 3;
    static constexpr RDGE::UInt8 UV_STAND_RIGHT_1 = 4;
    static constexpr RDGE::UInt8 UV_STAND_RIGHT_2 = 5;
    static constexpr RDGE::UInt8 UV_STAND_RIGHT_3 = 6;
    static constexpr RDGE::UInt8 UV_STAND_LEFT_1  = 7;
    static constexpr RDGE::UInt8 UV_STAND_LEFT_2  = 8;
    static constexpr RDGE::UInt8 UV_STAND_LEFT_3  = 9;

    std::shared_ptr<RDGE::Graphics::Layer2D>   m_layer;
    std::shared_ptr<RDGE::Graphics::SpriteSheet> m_spriteSheet;
    //std::shared_ptr<RDGE::Graphics::GLTexture> m_spriteSheet;
    //std::shared_ptr<RDGE::Graphics::GLTexture> m_spriteSheetBg;
    std::shared_ptr<RDGE::Graphics::Sprite>    m_chrono;

    std::vector<RDGE::Graphics::UVCoordinates> m_uvs;

    ChronoState m_chronoState;
    ChronoDirection m_chronoDirection;

    static constexpr RDGE::UInt32 ANIM_BLINK_DELAY_1 = 4000;
    static constexpr RDGE::UInt32 ANIM_BLINK_DELAY_2 = 4025;
    static constexpr RDGE::UInt32 ANIM_BLINK_DELAY_3 = 4050;
    static constexpr RDGE::UInt32 ANIM_BLINK_DELAY_4 = 4075;
    RDGE::UInt32 m_blinkCounter;

};

} // namespace glpong
