#include <rdge/application.hpp>
#include <rdge/window.hpp>
#include <rdge/color.hpp>
#include <rdge/font.hpp>
#include <rdge/surface.hpp>
#include <rdge/texture.hpp>
#include <rdge/graphics/point.hpp>
#include <rdge/util/timer.hpp>
#include <rdge/util/exception.hpp>

#include <SDL.h>

#include <string>
#include <iostream>

#include "game.hpp"


//#include <memory>

int main (int, char**)
{
    RDGE::Application app(SDL_INIT_EVERYTHING, 0, true);
    std::cout << "Starting pong" << std::endl;
    std::cout << RDGE::Application::BasePath() << std::endl;
    std::cout << "SDL version: " << app.SDLVersion() << std::endl;
    std::cout << "SDL_image version: " << app.SDLImageVersion() << std::endl;
    std::cout << "SDL_ttf version: " << app.SDLTTFVersion() << std::endl;

    try
    {
        RDGE::GameObjects::GameSettings settings;
        settings.window_title = "pong";
        settings.target_width = 1280;
        settings.target_height = 720;
        settings.target_fps = 200;
        settings.use_vsync = false;

        pong::PongGame game { settings };
        game.Run();
    }
    catch (const RDGE::SDLException& ex)
    {
        std::cout << "SDLException Raised!" << std::endl;
        std::cout << ex.what() << std::endl;
        std::cout << ex.SDLFunction() << std::endl;
        std::cout << ex.SDLError() << std::endl;
    }
    catch (const RDGE::Exception& ex)
    {
        std::cout << "Exception Raised!" << std::endl;
        std::cout << ex.File() << std::endl;
        std::cout << ex.FileName() << ":" << ex.Line() << " [" << ex.Function() << "]" << std::endl;
        std::cout << ex.what() << std::endl;
    }

    return 0;
}
