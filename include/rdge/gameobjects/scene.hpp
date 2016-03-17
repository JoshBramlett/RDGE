//! \headerfile <rdge/gameobjects/scene.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 01/30/2016
//! \bug

#pragma once

#include <memory>
#include <unordered_map>
#include <functional>

#include <SDL.h>

#include <rdge/types.hpp>
#include <rdge/window.hpp>
#include <rdge/gameobjects/ientity.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace GameObjects {

class Scene;

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
using SceneEventCallback = std::function<void(const Scene*, const SceneEventArgs&)>;

//! \class Scene
//! \brief Base class for a game scene
class Scene : public IEntity
{
public:

    // TODO: Should be passing the renderer, once we make it a shared pointer
    //
    // update:  To finish this thought, would it be better to have a subclass
    //          of the window object (e.g. SDLWindow, OpenGLWindow) where the
    //          renderer is setup there.  Alternatively the window could be a
    //          more light weight object with subclassed renderers

    //! \brief Scene ctor
    //! \details Scene setup is limited to setting the viewport, but derived
    //!          classes are responsible for loading any entities and assets
    //!          required by the scene
    //! \param [in] window Game window
    explicit Scene (const RDGE::Window& window);

    //! \brief Scene dtor
    virtual ~Scene (void) { }

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

    //! \brief Called within the event polling phase of the game loop
    //! \details Scenes are responsible for sending notifications to each
    //!          stored entity for them to process input events
    //! \param [in] event Polled SDL_Event
    virtual void HandleEvents (const SDL_Event& event);

    //! \brief Called within the update phase of the game loop
    //! \details Scenes are responsible for sending notifications to each
    //!          stored entity for them to update state logic
    //! \param [in] ticks Number of ticks that have occurred since the last update
    virtual void Update (RDGE::UInt32 ticks);

    //! \brief Called within the drawing phase of the game loop
    //! \details Scenes are responsible for sending notifications to each
    //!          stored entity for them to paint to the screen
    //! \param [in] window Window to render to
    virtual void Render (const RDGE::Window& window);

    //! \brief Entity Tag
    //! \returns Tag to signify entity group
    virtual std::string Tag (void) const
    {
        return "Scene";
    }

    //! \brief Register a callback function to receive events
    //! \details Single entry point registration, but can be called multiple
    //!          times to register for different event types.  Calling
    //!          multiple times with the same type will overwrite the
    //!          existing callback
    //! \param [in] type Type of event to subscribe to
    //! \param [in] handler Callback called when event is triggered
    void RegisterEventHandler (
                               SceneEventType     type,
                               SceneEventCallback handler
                              );

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
    //! \brief Add a game entity to the internal collection
    //! \details Should only be called once per entity, and there is no
    //!          replace functionality.  All updating should be performed
    //!          on the base object
    //! \param [in] id Unique identifier for the entity
    //! \param [in] entity Shared pointer to derived \ref Entity class
    //! \throws Entity could not be added to the collection
    virtual void AddEntity (
                            const std::string&       id,
                            std::shared_ptr<IEntity> entity
                           ) final;

    //! \brief Get a game entity from the internal collection
    //! \details A std::dynamic_pointer_cast will need to be performed on the
    //!          return value if implementation specific calls are needed
    //! \param [in] id Entity unique identifier
    //! \returns Shared pointer to class derived \ref Entity class
    //! \throws Requested entity is not in the collection
    virtual std::shared_ptr<IEntity> GetEntity (const std::string& id) const final;

    //! \brief Inform the subscriber the scene triggered an event
    //! \details Fires an event to the subscriber based on which event is
    //!          provided.  Derived classes should have logic in place to
    //!          signal the event and build the appropriate event arguments
    //! \param [in] type Type of event to fire
    //! \param [in] args Arguments to pass to the subscriber
    virtual void TriggerEvent (SceneEventType type, const SceneEventArgs& args) final;

private:
    //! \typedef EntityMap
    //! \brief Container type for all scene game objects
    using EntityMap = std::unordered_map<std::string, std::shared_ptr<IEntity>>;
    //! \typedef SubscriberMap
    //! \brief Container type for all scene subscribers
    using SubscriberMap = std::unordered_map<SceneEventType, SceneEventCallback>;

    EntityMap m_entities;
    SubscriberMap m_subscriptions;
};

} // namespace GameObjects
} // namespace RDGE
