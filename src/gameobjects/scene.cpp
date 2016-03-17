#include <rdge/gameobjects/scene.hpp>
#include <rdge/internal/exception_macros.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

Scene::Scene (const RDGE::Window&)
{
    // TODO: Why pass window to the base class?
}

Scene::Scene (Scene&& rhs) noexcept
{
    for (const auto& iter : rhs.m_entities)
    {
        m_entities.emplace(std::make_pair(iter.first, std::move(iter.second)));
    }
}

Scene&
Scene::operator= (Scene&& rhs) noexcept
{
    if (this != &rhs)
    {
        for (const auto& iter : m_entities)
        {
            m_entities.emplace(std::make_pair(iter.first, std::move(iter.second)));
        }
    }

    return *this;
}

void
Scene::HandleEvents (const SDL_Event& event)
{
    for (auto const& iter : m_entities)
    {
        iter.second->HandleEvents(event);
    }
}

void
Scene::Update (RDGE::UInt32 ticks)
{
    for (auto const& iter : m_entities)
    {
        iter.second->Update(ticks);
    }
}

void
Scene::Render (const Window& window)
{
    for (auto const& iter : m_entities)
    {
        iter.second->Render(window);
    }
}

void
Scene::AddEntity (
                  const std::string&       id,
                  std::shared_ptr<IEntity> entity
                 )
{
    auto emplaced = m_entities.emplace(id, std::move(entity));
    if (UNLIKELY(emplaced.second == false))
    {
        RDGE_THROW("Entity could not be inserted");
    }
}

std::shared_ptr<IEntity>
Scene::GetEntity (const std::string& id) const
{
    auto iter = m_entities.find(id);
    if (UNLIKELY(iter == m_entities.end()))
    {
        RDGE_THROW("Entity does not exist in map");
    }

    return iter->second;
}

void
Scene::RegisterEventHandler (
                             SceneEventType     type,
                             SceneEventCallback handler
                            )
{
    m_subscriptions[type] = handler;
}

void
Scene::TriggerEvent (
                     SceneEventType        type,
                     const SceneEventArgs& args
                    )
{
    auto iter = m_subscriptions.find(type);
    if (iter != m_subscriptions.end())
    {
        iter->second(this, args);
    }
}

} // namespace GameObjects
} // namespace RDGE
