#include <chrono/scenes/winery.hpp>
#include <chrono/asset_table.hpp>
#include <chrono/globals.hpp>

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/util.hpp>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;
using namespace rdge::tilemap;

WineryScene::WineryScene (void)
    : collision_graph({ 0.f, -9.8f })
{
    collision_graph.listener = this;

    auto tilemap = g_game.pack->GetAsset<tilemap::Tilemap>(rdge_asset_tilemap_winery);

    ///////////////////
    // Spawn Points
    ///////////////////

    {
        const auto& def = tilemap->layers[winery_layer_spawns];
        for (const auto& obj : def.objectgroup.objects)
        {
            this->spawn_points.emplace_back(ProcessSpawnPoint(obj));
        }
    }

    ///////////////////
    // Tile layers
    ///////////////////

    // TODO This is the total tile count, but more care should be taken into
    //      consideration to construct the buffer size because this should be
    //      set to the maximum number of tiles that could be drawn in a single
    //      frame.  Zoom should be considered, but in the general case this
    //      should be no more than the the number of tiles drawn for the maximum
    //      resolution supported.
    size_t tile_count = tilemap->grid.size.w * tilemap->grid.size.h;
    auto tile_size = static_cast<math::vec2>(tilemap->grid.cell_size) * g_game.ratios.base_to_screen;

    tile_batch = TileBatch(tile_count, tile_size);
    background_layers.reserve(6);
    background_layers.emplace_back(tilemap->CreateTileLayer(winery_layer_bg,
                                                      g_game.ratios.base_to_screen));
    background_layers.emplace_back(tilemap->CreateTileLayer(winery_layer_bg_overlay_01,
                                                      g_game.ratios.base_to_screen));
    background_layers.emplace_back(tilemap->CreateTileLayer(winery_layer_bg_overlay_02,
                                                      g_game.ratios.base_to_screen));
    background_layers.emplace_back(tilemap->CreateTileLayer(winery_layer_bg_overlay_03,
                                                      g_game.ratios.base_to_screen));
    background_layers.emplace_back(tilemap->CreateTileLayer(winery_layer_bg_overlay_04,
                                                      g_game.ratios.base_to_screen));
    background_layers.emplace_back(tilemap->CreateTileLayer(winery_layer_bg_overlay_05,
                                                      g_game.ratios.base_to_screen));

    foreground_layers.reserve(5);
    foreground_layers.emplace_back(tilemap->CreateTileLayer(winery_layer_fixtures_overlay_01,
                                                      g_game.ratios.base_to_screen));
    foreground_layers.emplace_back(tilemap->CreateTileLayer(winery_layer_fixtures_overlay_02,
                                                      g_game.ratios.base_to_screen));
    foreground_layers.emplace_back(tilemap->CreateTileLayer(winery_layer_wall_top,
                                                      g_game.ratios.base_to_screen));
    foreground_layers.emplace_back(tilemap->CreateTileLayer(winery_layer_wall_top_overlay_01,
                                                      g_game.ratios.base_to_screen));
    foreground_layers.emplace_back(tilemap->CreateTileLayer(winery_layer_wall_top_overlay_02,
                                                      g_game.ratios.base_to_screen));

    ///////////////////
    // Sprite layers
    ///////////////////

    {
        const auto& def = tilemap->layers[winery_layer_fixtures];
        uint16 sprite_capacity = def.objectgroup.objects.size() + 100;

        this->static_actors.reserve(sprite_capacity);
        this->sprite_layers.emplace_back(sprite_capacity);

        auto& layer = this->sprite_layers.back();
        layer.name = def.name;
        math::vec2 player_pos;
        Direction facing;
        for (const auto& spawn : this->spawn_points)
        {
            if (spawn.is_default)
            {
                player_pos = spawn.pos;
                facing = spawn.facing;
                break;
            }
        }

        // TODO clean this up
        SDL_assert(!player_pos.is_zero());
        player.Init(player_pos, layer, collision_graph);
        player.InitPosition(player_pos, facing);

        for (const auto& obj : def.objectgroup.objects)
        {
            // TODO Could set property on the obj to define that it's indeed static
            //
            // TODO StaticActors need to be initialized differently than other sprites.
            //      These objects have the collision object relative to their sprite.
            //      Dynamic sprites (especially those with animations) render their
            //      sprite relative to a collision object.  There should be a very
            //      explicit definition of those two types.
            if (obj.type == tilemap::ObjectType::SPRITE)
            {
                this->static_actors.emplace_back(obj,
                                                 *def.objectgroup.spritesheet,
                                                 layer,
                                                 collision_graph);
            }
        }
    }

    ///////////////////
    // World Collision
    ///////////////////

    {
        const auto& def = tilemap->layers[winery_layer_bg_collision];
        for (const auto& obj : def.objectgroup.objects)
        {
            if (obj.ext_type == "environment_static")
            {
                rigid_body_profile bprof;
                fixture_profile fprof;

                bprof.type = RigidBodyType::STATIC;
                bprof.position = obj.pos * g_game.ratios.base_to_world;
                auto body = collision_graph.CreateBody(bprof);

                if (obj.ext_data)
                {
                    // TODO This lookup is counter-productive.  Fixture profile
                    //      should be cached
                    const auto& ext_props = obj.ext_data->properties;
                    fprof.density = ext_props.GetFloat("density");
                    fprof.friction = ext_props.GetFloat("friction");
                    fprof.restitution = ext_props.GetFloat("restitution");
                    fprof.is_sensor = ext_props.GetBool("is_sensor");
                    fprof.filter.category = ext_props.GetInt("cgroup");
                    fprof.filter.mask = ext_props.GetInt("cmask");

                    fprof.override_color = true;
                    fprof.wireframe = obj.ext_data->color;
                }
                else
                {
                    fprof.density = 1.f;
                    fprof.friction = 0.2f;
                    fprof.restitution = 0.0f;
                    fprof.is_sensor = false;
                    fprof.filter.category = chrono_collision_category_environment_static;
                    fprof.filter.mask = chrono_collision_category_all_hitbox;
                }

                if (obj.type == tilemap::ObjectType::POLYGON)
                {
                    auto p = obj.GetPolygon(g_game.ratios.base_to_world, true);
                    fprof.shape = &p;
                    body->CreateFixture(fprof);
                }
            }
        }
    }

    debug::AddWidget(this);
    debug::settings::show_overlay = true;
    debug::settings::physics::draw_fixtures = false;
}

void
WineryScene::Initialize (void)
{
    ILOG() << "WineryScene::Initialize";
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph, g_game.ratios.world_to_screen);

    for (auto& layer : this->sprite_layers)
    {
        debug::RegisterGraphics(&layer);
    }

    for (auto& layer : this->background_layers)
    {
        debug::RegisterGraphics(&layer);
    }

    for (auto& layer : this->foreground_layers)
    {
        debug::RegisterGraphics(&layer);
    }
}

void
WineryScene::Terminate (void)
{
    ILOG() << "WineryScene::Terminate";
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);
    debug::ClearGraphics();
}

void
WineryScene::Activate (void)
{
    ILOG() << "WineryScene::Activate";
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph, g_game.ratios.world_to_screen);

    for (auto& layer : this->sprite_layers)
    {
        debug::RegisterGraphics(&layer);
    }

    for (auto& layer : this->background_layers)
    {
        debug::RegisterGraphics(&layer);
    }

    for (auto& layer : this->foreground_layers)
    {
        debug::RegisterGraphics(&layer);
    }
}

void
WineryScene::Hibernate (void)
{
    ILOG() << "WineryScene::Hibernate";
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);
    debug::ClearGraphics();
}

void
WineryScene::OnEvent (const Event& event)
{
    player.OnEvent(event);
    if (event.IsKeyboardEvent())
    {
        const auto& args = event.GetKeyboardEventArgs();
        if (!args.IsRepeating())
        {
            if (args.IsKeyPressed() && args.Key() == KeyCode::BACKSPACE)
            {
                QueueCustomEvent(g_game.custom_events.push_scene, 0);
            }
        }
    }
}

void
WineryScene::OnUpdate (const delta_time& dt)
{
    collision_graph.Step(1.f / 60.f);
    player.OnUpdate(dt);
}

void
WineryScene::OnRender (void)
{
    camera.SetPosition(player.GetWorldCenter() * g_game.ratios.world_to_screen);
    camera.Update();

    tile_batch.SetView(camera);
    sprite_batch.SetView(camera);

    for (auto& layer : this->background_layers)
    {
        layer.Draw(tile_batch, camera);
    }

    for (auto& layer : this->sprite_layers)
    {
        layer.Draw(sprite_batch, camera);
    }

    tile_batch.depth = 0.0f;
    for (auto& layer : this->foreground_layers)
    {
        layer.Draw(tile_batch, camera);
    }

    // debug drawing
    debug::SetProjection(camera.combined);
}

void
WineryScene::OnContactStart (Contact* c)
{
    rdge::Unused(c);
    //std::cout << "OnContactStart" << std::endl;
}

void
WineryScene::OnContactEnd (Contact* c)
{
    rdge::Unused(c);
    //std::cout << "OnContactEnd" << std::endl;
}

void
WineryScene::OnPreSolve (Contact* c, const collision_manifold& mf)
{
    rdge::Unused(c);
    rdge::Unused(mf);
    //std::cout << "OnPreSolve" << std::endl;
}

void
WineryScene::OnPostSolve (Contact* c)
{
    rdge::Unused(c);
    //std::cout << "OnPostSolve" << std::endl
              //<< c->manifold << std::endl
              //<< c->impulse << std::endl;
}

void
WineryScene::OnDestroyed (Fixture*)
{ }

void
WineryScene::UpdateWidget (void)
{
#if 0
    if (!this->show_widget)
    {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    float fb_width = static_cast<float>(io.DisplaySize.x);
    float fb_height = static_cast<float>(io.DisplaySize.y);
    float menu_width = 200.f;

    ImGui::SetNextWindowPos(ImVec2(fb_width - (menu_width + 20.f), 25.f));
    ImGui::SetNextWindowSize(ImVec2(menu_width, fb_height - 50.f),
                             ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("WineryScene", &this->show_widget))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Player");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("pos: %s", rdge::to_string(player.GetWorldCenter()).c_str());
    ImGui::Text("vel: %s", rdge::to_string(player.body->linear.velocity).c_str());
    ImGui::Unindent(15.f);
    ImGui::Separator();

    ImGui::Text("Duck");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("pos: %s", rdge::to_string(duck.GetWorldCenter()).c_str());
    ImGui::Text("vel: %s", rdge::to_string(duck.body->linear.velocity).c_str());
    ImGui::SliderFloat("#one", &duck.kb_impulse, 5.f, 100.f, "impulse = %.3f");
    ImGui::SliderFloat("#two", &duck.kb_damping, 5.f, 100.f, "damping = %.3f");
    ImGui::Unindent(15.f);
    ImGui::Separator();

    auto ab = player.GetWorldCenter() - duck.GetWorldCenter();
    float dot = math::dot(ab, duck.body->linear.velocity);

    float dot_normal_vel = math::dot(player.normal, player.body->linear.velocity);

    ImGui::Text("Misc");
    ImGui::Spacing();
    ImGui::Indent(15.f);
    ImGui::Text("dot: %f", dot);
    ImGui::Text("dnv: %f", dot_normal_vel);
    ImGui::Unindent(15.f);
    ImGui::Separator();



    ImGui::End();
#endif
}

void
WineryScene::OnWidgetCustomRender (void)
{ }
