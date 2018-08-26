#include <chrono/scenes/scene_manager.hpp>
#include <chrono/types.hpp>
#include <chrono/globals.hpp>

// scene includes
#include "overworld.hpp"
#include "winery.hpp"

#include <rdge/type_traits.hpp>
#include <rdge/gameobjects/iscene.hpp>
#include <rdge/debug/assert.hpp>

#include <array>
#include <memory>

namespace perch {

namespace {

std::array<std::shared_ptr<rdge::IScene>, chrono_scene_count> s_scenes;

std::shared_ptr<rdge::IScene>
GetScene (chrono_scene_id scene_id)
{
    static_assert(rdge::enum_sanity_check(chrono_scene_winery, chrono_scene_count),
                  "chrono_scene_id sanity check failed");
    RDGE_ASSERT(scene_id > chrono_scene_none);
    RDGE_ASSERT(scene_id < chrono_scene_count);

    if (!s_scenes[scene_id])
    {
        switch (scene_id)
        {
        case chrono_scene_overworld:
            s_scenes[scene_id] = std::make_shared<OverworldScene>();
            break;
        case chrono_scene_winery:
            s_scenes[scene_id] = std::make_shared<WineryScene>();
            break;
        default:
            RDGE_ASSERT(false);
            break;
        }
    }

    return s_scenes[scene_id];
}

} // anonymous namespace

void
PushScene (chrono_scene_id scene_id)
{
    g_game.game->PushScene(GetScene(scene_id));
}

void
PopScene (void)
{
    g_game.game->PopScene();
}

void
SwapScene (chrono_scene_id scene_id)
{
    g_game.game->SwapScene(GetScene(scene_id));
}

void
DestroyAllScenes (void)
{
    for (auto& scene : s_scenes)
    {
        scene.reset();
    }
}

} // namespace perch
