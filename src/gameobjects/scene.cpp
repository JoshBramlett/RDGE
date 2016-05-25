#include <rdge/gameobjects/scene.hpp>
#include <rdge/internal/exception_macros.hpp>
#include <rdge/internal/logger_macros.hpp>

#include <utility>
#include <ostream>
#include <sstream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

namespace {
    std::ostream& operator<< (std::ostream& os, SceneEventType type)
    {
        switch (type)
        {
        case SceneEventType::RequestingPop:
            return os << "RequestingPop";
        case SceneEventType::RequestingPush:
            return os << "RequestingPush";
        default:
            return os << "Unknown";
        }

        return os;
    }
} // anonymous namespace

Scene::Scene (void)
{
    DLOG("Constructing Scene object");
}

Scene::~Scene (void)
{
    DLOG("Destroying Scene object");
}

Scene::Scene (Scene&& rhs) noexcept
    : m_entities(std::move(rhs.m_entities))
    , m_subscriptions(std::move(rhs.m_subscriptions))
{ }

Scene&
Scene::operator= (Scene&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_entities = std::move(rhs.m_entities);
        m_subscriptions = std::move(rhs.m_subscriptions);
    }

    return *this;
}

void
Scene::ProcessEventPhase (const SDL_Event& event)
{
    for (auto const& iter : m_entities)
    {
        iter.second->HandleEvents(event);
    }
}

void
Scene::ProcessUpdatePhase (RDGE::UInt32 ticks)
{
    for (auto const& iter : m_entities)
    {
        iter.second->Update(ticks);
    }
}

void
Scene::ProcessRenderPhase (void)
{
    for (auto const& iter : m_layers)
    {
        iter.second->Render();
    }
}

void
Scene::RegisterEventHandler (SceneEventType type, SceneEventCallback handler)
{
    std::stringstream ss;
    ss << "Registering Scene EventHandler type=" << type;
    DLOG(ss.str());

    m_subscriptions[type] = handler;
}

void
Scene::AddEntity (const std::string& id, const std::shared_ptr<IEntity>& entity)
{
    DLOG("Adding entity to scene.  id=" + id);

    auto emplaced = m_entities.emplace(std::make_pair(id, entity));
    if (UNLIKELY(emplaced.second == false))
    {
        RDGE_THROW("Entity could not be inserted.  id=" + id);
    }
}

void
Scene::AddLayer (const std::string& id, const std::shared_ptr<RDGE::Graphics::Layer>& layer)
{
    DLOG("Adding layer to scene.  id=" + id);

    auto emplaced = m_layers.emplace(std::make_pair(id, layer));
    if (UNLIKELY(emplaced.second == false))
    {
        RDGE_THROW("Layer could not be inserted.  id=" + id);
    }
}

std::shared_ptr<IEntity>
Scene::GetEntity (const std::string& id) const
{
    auto iter = m_entities.find(id);
    if (UNLIKELY(iter == m_entities.end()))
    {
        RDGE_THROW("Entity does not exist in map.  id=" + id);
    }

    return iter->second;
}

std::shared_ptr<RDGE::Graphics::Layer>
Scene::GetLayer (const std::string& id) const
{
    auto iter = m_layers.find(id);
    if (UNLIKELY(iter == m_layers.end()))
    {
        RDGE_THROW("Layer does not exist in map.  id=" + id);
    }

    return iter->second;
}

void
Scene::TriggerEvent (SceneEventType type, const SceneEventArgs& args)
{
    auto iter = m_subscriptions.find(type);
    if (iter != m_subscriptions.end())
    {
        iter->second(this, args);
    }
}

} // namespace GameObjects
} // namespace RDGE
