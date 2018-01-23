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
    debug::settings::draw_physics_fixtures = false;

    ///////////////////
    // Background layer
    ///////////////////

    auto tilemap = g_game.pack->GetTilemap(rdge_asset_tilemap_overworld);

    // TODO This is the total tile count, but more care should be taken into
    //      consideration to construct the buffer size because this should be
    //      set to the maximum number of tiles that could be drawn in a single
    //      frame.  Zoom should be considered, but in the general case this
    //      should be no more than the the number of tiles drawn for the maximum
    //      resolution supported.
    size_t tile_count = tilemap.grid.size.w * tilemap.grid.size.h;
    auto tile_size = static_cast<math::vec2>(tilemap.grid.cell_size) * g_game.asset_scale;
    tile_batch = TileBatch(tile_count, tile_size);

    {
        const auto& bg_0 = tilemap.layers[overworld_layer_bg];
        const auto& bg_1 = tilemap.layers[overworld_layer_bg_overlay_1];

        auto table_id_0 = tilemap.sheets[bg_0.tileset_index].table_id;
        auto table_id_1 = tilemap.sheets[bg_1.tileset_index].table_id;
        SDL_assert(table_id_0 == table_id_1);

        auto tileset = g_game.pack->GetAsset<Tileset>(table_id_0);
        tile_layers.emplace_back(tilemap.grid, bg_0, *tileset, g_game.asset_scale);
        tile_layers.emplace_back(tilemap.grid, bg_1, *tileset, g_game.asset_scale);
    }

    auto sheet = g_game.pack->GetAsset<SpriteSheet>(rdge_asset_spritesheet_player);
    sprite_batch = std::make_shared<SpriteBatch>();
    sprite_layers.emplace_back(sprite_batch);
    auto& layer = sprite_layers.back();
    {
    math::vec3 p(2712, -2468, 0.f);
    auto t = std::make_shared<Texture>(*sheet->surface);
    auto& region = sheet->regions[frame_player_attack_back_2].value;
    auto sprite = std::make_shared<Sprite>(p, region.size * g_game.asset_scale, t, region.coords);
    layer.AddSprite(sprite);
    }

    auto tileset = g_game.pack->GetAsset<Tileset>(rdge_asset_tileset_overworld_bg);
    {
    math::vec3 p(2612, -2468, 0.f);
    auto t = std::make_shared<Texture>(*tileset->surface);
    auto& coords = tileset->tiles[29];
    auto sprite = std::make_shared<Sprite>(p, tileset->tile_size * g_game.asset_scale, t, coords);
    layer.AddSprite(sprite);
    }


    //layer.AddSprite(std::make_shared<Sprite>(p, s, color::RED));
}

void
OverworldScene::Initialize (void)
{
    debug::RegisterCamera(&camera);
    debug::RegisterPhysics(&collision_graph, g_game.ppm);
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
    debug::RegisterPhysics(&collision_graph, g_game.ppm);
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
    rdge::Unused(event);
}

void
OverworldScene::OnUpdate (const delta_time& dt)
{
    rdge::Unused(dt);
}

void
OverworldScene::OnRender (void)
{
    //camera.SetPosition(player.GetWorldCenter() * g_game.ppm);
    //camera.Update();

    camera.SetPosition(math::vec2(2712, -2468));
    //camera.SetPosition(math::vec2(-1024, 1024));
    camera.Update();
    for (auto& layer : this->tile_layers)
    {
        layer.Draw(tile_batch, camera);
    }

    sprite_batch->SetProjection(camera.combined);
    for (auto& layer : this->sprite_layers)
    {
        layer.Draw();
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
