#include <rdge/application.hpp>
#include <rdge/config.hpp>
#include <rdge/util/exception.hpp>

#include "gui.hpp"

int main()
{
    try
    {
        RDGE::game_settings settings;
        settings.enable_fonts = true;
        settings.enable_png = true;
        settings.use_vsync = true;
        settings.window_title = "RDGE GUI Example";
        RDGE::Application app { settings };

        gui::GUIGame game { settings };
        game.Run();
    }
    catch (const RDGE::Exception& ex)
    {
        std::cout << "RDGE::Exception Raised!" << std::endl
                  << "    " << ex.what() << std::endl
                  << "    " << ex.FileName() << ":" << ex.Line()
                  << " [" << ex.Function() << "]" << std::endl;
    }
    catch (const RDGE::SDLException& ex)
    {
        std::cout << "RDGE::SDLException Raised!" << std::endl
                  << "    " << ex.what() << std::endl
                  << "    " << ex.SDLFunction() << std::endl
                  << "    " << ex.SDLError() << std::endl;
    }
    catch (const RDGE::GLException& ex)
    {
        std::cout << "RDGE::GLException Raised!" << std::endl
                  << "    " << ex.what() << std::endl
                  << "    " << ex.GLFunction() << std::endl
                  << "    " << ex.GLErrorCodeString() << std::endl;
    }

    return 0;
}
