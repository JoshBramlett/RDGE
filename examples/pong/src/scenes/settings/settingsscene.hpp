#pragma once

#include <rdge/types.hpp>
#include <rdge/texture.hpp>
#include <rdge/window.hpp>
#include <rdge/gameobjects/scene.hpp>
#include <rdge/controls/control.hpp>

#include <memory>

namespace pong {

class SettingsScene final : public RDGE::GameObjects::Scene
{
public:
    explicit SettingsScene (const RDGE::Window& window);

    virtual ~SettingsScene (void) { }

private:
    void OnButtonClick (const RDGE::Controls::ControlEventArgs& args);

    std::shared_ptr<RDGE::Texture> m_buttonsTexture;
};

} // namespace pong
