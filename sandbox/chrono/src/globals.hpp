#pragma once

namespace rdge {
class Game;
class PackFile;
}

struct chrono_globals
{
    rdge::Game* game;
    rdge::PackFile* pack;

    float ppm;
    float inv_ppm;
};

extern chrono_globals g_game;
