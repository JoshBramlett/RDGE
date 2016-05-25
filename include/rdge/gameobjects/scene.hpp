//! \headerfile <rdge/gameobjects/scene.hpp>
//! \author Josh Bramlett
//! \version 0.0.7
//! \date 05/21/2016

#pragma once

#include <rdge/types.hpp>
#include <rdge/graphics/layers/layer.hpp>
#include <rdge/gameobjects/ientity.hpp>

#include <SDL.h>

#include <memory>
#include <unordered_map>
#include <functional>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

//! \enum SceneEventType
//! \brief Supported event types
enum class SceneEventType : RDGE::UInt8
{
    //! \brief Scene is requesting to be terminated
    RequestingPop = 1,
    //! \brief Scene is requesting another scene be pushed on the stack
    RequestingPush
};

//! \struct SceneEventArgs
//! \brief Arguments sent with \ref SceneEventCallback
struct SceneEventArgs
{
    //! \var id Identifier of the signaled event
    std::string    id;
    //! \var type Type of scene event fired
    SceneEventType type;
};

//! \typedef SceneEventCallback
//! \brief Callback subscriber for scene events
using SceneEventCallback = std::function<void(void*, const SceneEventArgs&)>;

//! \class Scene
//! \brief Base class for a game scene
//! \details A scene represents a grouping of layers and game entities and
//!          provides a mechanism to dispatch the different phases of the game
//!          loop.  The base class cannot be instantiated, and allows
//!          derived classes to override most methods.  A scene should be
//!          thought of as the entire game setting at a given time.  For example,
//!          derived scenes could be CombatScene, OverworldScene, Level1Scene, etc.
class Scene
{
public:
    //! \brief Called within the event polling phase of the game loop
    //! \details Scenes are responsible for sending notifications to each
    //!          stored entity for them to process input events.
    //! \param [in] event Polled SDL_Event
    virtual void ProcessEventPhase (const SDL_Event& event);

    //! \brief Called within the update phase of the game loop
    //! \details Scenes are responsible for sending notifications to each
    //!          stored entity for them to update state logic.
    //! \param [in] ticks Number of ticks that have occurred since the last update
    virtual void ProcessUpdatePhase (RDGE::UInt32 ticks);

    //! \brief Called within the drawing phase of the game loop
    //! \details Scenes are responsible for sending notifications to each
    //!          stored layer to draw their renderables.
    virtual void ProcessRenderPhase (void);

    //! \brief Register a callback function to receive events
    //! \details Single entry point registration, but can be called multiple
    //!          times to register for different event types.  Calling
    //!          multiple times with the same type will overwrite the
    //!          existing callback
    //! \param [in] type Type of event to subscribe to
    //! \param [in] handler Callback called when event is triggered
    void RegisterEventHandler (SceneEventType type, SceneEventCallback handler);

    //! \brief Called when the scene is pushed on the stack
    //! \details Allows scene to perform any necessary setup prior to being ran
    //!          This is separte from the constructor as scene objects can be
    //!          cached for re-use
    //! \note It's not recommended this be called directly
    virtual void Initialize (void) { }

    //! \brief Called when the scene is popped off the stack
    //! \details Allows scene to perform any necessary cleanup.  This is separate
    //!          from the destructor as scene objects can be cached for re-use
    //! \note It's not recommended this be called directly
    virtual void Terminate (void) { }

    //! \brief Called when the scene is pushed lower on the stack
    //! \details All scene objects on the stack that are not at the top (current)
    //!          position will be in a paused state
    virtual void Pause (void) { }

    //! \brief Called when the scene is on the stack and becomes current
    //! \details The scene which was once in a paused state is brought to the
    //!          forefront and is the current (rendered) state
    virtual void Resume (void) { }

    // TODO: Implement?
    //virtual void Load (void);
    //virtual void Save (void);

protected:
    //! \brief Scene ctor
    //! \details It's recommended derived classes load all entities and layers,
    //!          and register all event handlers.
    Scene (void);

    //! \brief Scene dtor
    virtual ~Scene (void);

    //! \brief Scene Copy ctor
    //! \details Non-copyable
    Scene (const Scene&) = delete;

    //! \brief Scene Move ctor
    //! \details Move semantics performed on the collection
    Scene (Scene&& rhs) noexcept;

    //! \brief Scene Copy Assignment Operator
    //! \details Non-copyable
    Scene& operator= (const Scene&) = delete;

    //! \brief Scene Move Assignment Operator
    //! \details Move semantics performed on the collection
    Scene& operator= (Scene&& rhs) noexcept;

    //! \brief Add a game entity to the internal collection
    //! \details Should only be called once per entity, and will throw if an
    //!          identical key is attempted (or if there is a hash collision).
    //! \param [in] id Unique identifier for the entity
    //! \param [in] entity Shared pointer to derived \ref Entity class
    //! \throws RDGE::Exception Entity could not be added to the collection
    virtual void AddEntity (const std::string& id, const std::shared_ptr<IEntity>& entity) final;

    //! \brief Add a drawable layer to the internal collection
    //! \details Should only be called once per layer, and will throw if an
    //!          identical key is attempted (or if there is a hash collision).
    //! \param [in] id Unique identifier for the layer
    //! \param [in] layer Shared pointer to derived layer
    //! \throws RDGE::Exception Layer could not be added to the collection
    virtual void AddLayer (
                           const std::string& id,
                           const std::shared_ptr<RDGE::Graphics::Layer>& layer
                          ) final;

    //! \brief Get a game entity from the internal collection
    //! \details A std::dynamic_pointer_cast will need to be performed on the
    //!          return value if implementation specific calls are needed
    //! \param [in] id Entity unique identifier
    //! \returns Shared pointer to class derived \ref Entity class
    //! \throws RDGE::Exception Requested entity is not in the collection
    virtual std::shared_ptr<IEntity> GetEntity (const std::string& id) const final;

    //! \brief Get a drawable layer from the internal collection
    //! \details A std::dynamic_pointer_cast will need to be performed on the
    //!          return value if implementation specific calls are needed
    //! \param [in] id Layer unique identifier
    //! \returns Shared pointer of the layer
    //! \throws RDGE::Exception Requested layer is not in the collection
    virtual std::shared_ptr<RDGE::Graphics::Layer> GetLayer (const std::string& id) const final;

    //! \brief Inform the subscriber the scene triggered an event
    //! \details Fires an event to the subscriber based on which event is
    //!          provided.  Derived classes should have logic in place to
    //!          signal the event and build the appropriate event arguments
    //! \param [in] type Type of event to fire
    //! \param [in] args Arguments to pass to the subscriber
    virtual void TriggerEvent (SceneEventType type, const SceneEventArgs& args) final;

private:
    //! \typedef EntityMap Container type for all scene entities
    using EntityMap = std::unordered_map<std::string, std::shared_ptr<IEntity>>;
    //! \typedef SubscriberMap Container type for all scene event subscribers
    using SubscriberMap = std::unordered_map<SceneEventType, SceneEventCallback>;
    //! \typedef LayerMap Container type for scene layers
    using LayerMap = std::unordered_map<std::string, std::shared_ptr<RDGE::Graphics::Layer>>;

    EntityMap     m_entities;
    SubscriberMap m_subscriptions;
    LayerMap      m_layers;
};

} // namespace GameObjects
} // namespace RDGE
