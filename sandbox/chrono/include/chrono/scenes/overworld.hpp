#pragma once

#include <rdge/core.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects.hpp>
#include <rdge/graphics.hpp>
#include <rdge/physics.hpp>
#include <rdge/debug.hpp>

#include <vector>
#include <memory>

class OverworldScene
    : public rdge::IScene
    , public rdge::physics::GraphListener
    , public rdge::debug::IWidget
{
public:
    OverworldScene (void);
    ~OverworldScene (void) noexcept = default;

    //!@{ IScene - Scene Transitions
    void Initialize (void) override;
    void Terminate (void) override;
    void Hibernate (void) override;
    void Activate (void) override;
    //!@}

    //!@{ IScene - Game Loop
    void OnEvent (const rdge::Event& event) override;
    void OnUpdate (const rdge::delta_time& dt) override;
    void OnRender (void) override;
    //!@}

    //!@{ GraphListener - Physics Events
    void OnContactStart (rdge::physics::Contact*) override;
    void OnContactEnd (rdge::physics::Contact*) override;
    void OnPreSolve (rdge::physics::Contact*, const rdge::physics::collision_manifold&) override;
    void OnPostSolve (rdge::physics::Contact*) override;
    void OnDestroyed (rdge::physics::Fixture*) override;
    //!@}

    //!@{ IWidget - Debug Drawing
    void UpdateWidget (void) override;
    void OnWidgetCustomRender (void) override;
    //!@}

public:
    rdge::physics::CollisionGraph collision_graph;
    rdge::OrthographicCamera camera;
    std::shared_ptr<rdge::SpriteBatch> sprite_batch;
    rdge::TileBatch tile_batch;
    std::vector<rdge::NewSpriteLayer> new_sprite_layers;
    std::vector<rdge::SpriteLayer> sprite_layers;
    std::vector<rdge::TileLayer> tile_layers;

    // debug
    bool show_widget = true;
};
