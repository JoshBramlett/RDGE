#pragma once

#include <rdge/core.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects/iscene.hpp>
#include <rdge/graphics.hpp>

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

    rdge::SpriteLayer stall_layer;
    std::shared_ptr<rdge::SpriteGroup> water_front;
    std::shared_ptr<rdge::SpriteGroup> water_back;
};
