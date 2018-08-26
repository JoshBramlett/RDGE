#pragma once

#define CREATE_TILE_LAYER(c, m, id) do {                                    \
    c.emplace_back(m->CreateTileLayer(id, g_game.ratios.base_to_screen));   \
} while (false)

