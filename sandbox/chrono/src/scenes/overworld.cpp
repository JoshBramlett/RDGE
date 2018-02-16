#include <chrono/scenes/overworld.hpp>
#include <chrono/asset_table.hpp>
#include <chrono/globals.hpp>

#include <rdge/assets.hpp>
#include <rdge/math.hpp>
#include <rdge/util.hpp>

using namespace rdge;
using namespace rdge::math;
using namespace rdge::physics;
using namespace rdge::tilemap;

OverworldScene::OverworldScene (void)
    : collision_graph({ 0.f, -9.8f })
{
    collision_graph.listener = this;

    debug::AddWidget(this);
    debug::settings::show_overlay = true;
    debug::settings::draw_physics_fixtures = true;

    auto tilemap = g_game.pack->GetTilemap(rdge_asset_tilemap_overworld);

    ///////////////////
    // Tile layers
    ///////////////////

    // TODO This is the total tile count, but more care should be taken into
    //      consideration to construct the buffer size because this should be
    //      set to the maximum number of tiles that could be drawn in a single
    //      frame.  Zoom should be considered, but in the general case this
    //      should be no more than the the number of tiles drawn for the maximum
    //      resolution supported.
    size_t tile_count = tilemap.grid.size.w * tilemap.grid.size.h;
    auto tile_size = static_cast<math::vec2>(tilemap.grid.cell_size) * g_game.ratios.base_to_screen;

    tile_batch = TileBatch(tile_count, tile_size);
    tile_layers.reserve(2);
    tile_layers.emplace_back(tilemap.CreateTileLayer(overworld_layer_bg,
                                                     g_game.ratios.base_to_screen));
    tile_layers.emplace_back(tilemap.CreateTileLayer(overworld_layer_bg_overlay_1,
                                                     g_game.ratios.base_to_screen));

    ///////////////////
    // Sprite layers
    ///////////////////

#if 0
    sprite_layers.emplace_back(tilemap.CreateSpriteLayer(overworld_layer_bg_sprites,
                                                         g_game.ratios.base_to_screen));

    math::vec2 player_pos(678.f, -617.f);
    player.InitPhysics(collision_graph, player_pos);
    player.InitGraphics(sprite_layers.back(), player_pos);

#else
    const auto& def = tilemap.layers[overworld_layer_bg_sprites];
    uint16 sprite_capacity = def.objectgroup.objects.size() + 100;

    this->static_actors.reserve(sprite_capacity);
    this->sprite_layers.emplace_back(sprite_capacity);

    // FIXME Polygons must be added before circles or all hell breaks loose
    auto& layer = this->sprite_layers.back();
    math::vec2 player_pos(678.f, -617.f);
    player.Init(player_pos, layer, collision_graph);

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
#endif
}

void
OverworldScene::Initialize (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph, g_game.ratios.world_to_screen);
}

void
OverworldScene::Terminate (void)
{
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);
}

void
OverworldScene::Activate (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph, g_game.ratios.world_to_screen);
}

void
OverworldScene::Hibernate (void)
{
    debug::RegisterCamera(nullptr);
    debug::RegisterPhysics(nullptr);
}

void
OverworldScene::OnEvent (const Event& event)
{
    player.OnEvent(event);
}

void
OverworldScene::OnUpdate (const delta_time& dt)
{
    player.OnUpdate(dt);
    collision_graph.Step(1.f / 60.f);
}

void
OverworldScene::OnRender (void)
{
    camera.SetPosition(player.GetWorldCenter() * g_game.ratios.world_to_screen);
    camera.Update();

    for (auto& layer : this->tile_layers)
    {
        layer.Draw(tile_batch, camera);
    }

    for (auto& layer : this->sprite_layers)
    {
        layer.Draw(sprite_batch, camera);
    }

    // debug drawing
    debug::SetProjection(camera.combined);
}

void
OverworldScene::OnContactStart (Contact* c)
{
    rdge::Unused(c);
    //std::cout << "OnContactStart" << std::endl;
}

void
OverworldScene::OnContactEnd (Contact* c)
{
    rdge::Unused(c);
    //std::cout << "OnContactEnd" << std::endl;
}

void
OverworldScene::OnPreSolve (Contact* c, const collision_manifold& mf)
{
    rdge::Unused(c);
    rdge::Unused(mf);
    //std::cout << "OnPreSolve" << std::endl;
}

void
OverworldScene::OnPostSolve (Contact* c)
{
    rdge::Unused(c);
    //std::cout << "OnPostSolve" << std::endl
              //<< c->manifold << std::endl
              //<< c->impulse << std::endl;
}

void
OverworldScene::OnDestroyed (Fixture*)
{ }

void
OverworldScene::UpdateWidget (void)
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
    if (!ImGui::Begin("OverworldScene", &this->show_widget))
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
OverworldScene::OnWidgetCustomRender (void)
{ }
