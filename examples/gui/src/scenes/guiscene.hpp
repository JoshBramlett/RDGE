#pragma once

#include <rdge/types.hpp>
#include <rdge/glwindow.hpp>
#include <rdge/gameobjects/scene.hpp>
#include <rdge/graphics/layers/layer2d.hpp>
#include <rdge/controls/button.hpp>

namespace gui {

class GUIScene final : public RDGE::GameObjects::Scene
{
public:
    explicit GUIScene (RDGE::GLWindow* window);

    virtual ~GUIScene (void) { }

    virtual void ProcessUpdatePhase (RDGE::UInt32 ticks) override;

private:

    void OnButtonClick (const RDGE::Controls::ControlEventArgs& args);
    void OnButtonDoubleClick (const RDGE::Controls::ControlEventArgs& args);
    void OnMouseLeave (const RDGE::Controls::ControlEventArgs& args);
    void OnMouseEnter (const RDGE::Controls::ControlEventArgs& args);
    void OnMouseDown (const RDGE::Controls::ControlEventArgs& args);
    void OnFocusGained (const RDGE::Controls::ControlEventArgs& args);
    void OnFocusLost (const RDGE::Controls::ControlEventArgs& args);

    std::shared_ptr<RDGE::Graphics::Layer2D> m_layer;
    std::shared_ptr<RDGE::Controls::Button>  m_button;
};

} // namespace glpong
