#pragma once

#include <rdge/core.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects/iscene.hpp>
#include <rdge/graphics.hpp>

#include <memory>

class ShootingGalleryScene : public rdge::IScene
{
public:
    ShootingGalleryScene (void);

    void Initialize (void) override;
    void Terminate (void) override;

    void Hibernate (void) override;
    void Activate (void) override;

    void OnEvent (const rdge::Event& event) override;
    void OnUpdate (rdge::uint32 ticks) override;
    void OnRender (void) override;

public:
    rdge::OrthographicCamera camera;

    std::shared_ptr<rdge::SpriteBatch> render_target;
    rdge::SpriteLayer bg_layer;
    rdge::SpriteLayer p3_layer;
    rdge::SpriteLayer p2_layer;
    rdge::SpriteLayer p1_layer;
    rdge::SpriteLayer curtain_layer;

    std::shared_ptr<rdge::SpriteGroup> water_front;
    std::shared_ptr<rdge::SpriteGroup> water_back;
};
