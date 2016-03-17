#pragma once

#include <rdge/types.hpp>
#include <rdge/gameobjects/ientity.hpp>

namespace pong {

using namespace RDGE::GameObjects;

class Net final : public IEntity
{
public:
    explicit Net (void);

    virtual ~Net (void) { }

    void HandleEvents (const SDL_Event&) { }

    void Update (RDGE::UInt32) { }

    void Render (const RDGE::Window& window);

    std::string Tag (void) const
    {
        return "Scenery";
    }

private:
    RDGE::Int32 m_netWidth;
    RDGE::Int32 m_netHeight;
    RDGE::Int32 m_netSpacing;
    RDGE::Int32 m_netCount;
};

} // namespace pong
