#pragma once

#include <rdge/types.hpp>
#include <rdge/texture.hpp>
#include <rdge/window.hpp>
#include <rdge/gameobjects/scene.hpp>
#include <rdge/controls/control.hpp>

#include <memory>

namespace pong {

class MenuScene final : public RDGE::GameObjects::Scene
{
public:
    explicit MenuScene (const RDGE::Window& window);

    virtual ~MenuScene (void) { }

private:
    void OnButtonClick (const RDGE::Controls::ControlEventArgs& args);

    std::shared_ptr<RDGE::Texture> m_buttonsTexture;
};

} // namespace pong
