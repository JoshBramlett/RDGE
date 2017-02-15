#pragma once

#include <rdge/core.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects/iscene.hpp>
#include <rdge/graphics.hpp>

#include <memory>

class TestScene : public rdge::IScene
{
public:
    TestScene (void);

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
    rdge::SpriteLayer background;
};
