#include <rdge/application.hpp>
#include <rdge/config.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/logger.hpp>

#include <SDL.h>

#include "glpong.hpp"

int main()
{
    try
    {
        //auto settings = RDGE::ReadConfigFile("config.json");
        RDGE::game_settings settings;
        settings.enable_fonts = true;
        RDGE::Application app { settings };

        glpong::GLPongGame game { settings };
        game.Run();
    }
    catch (const RDGE::Exception& ex)
    {

    }
    catch (const RDGE::SDLException& ex)
    {

    }
    catch (const RDGE::GLException& ex)
    {

    }

    return 0;
}
