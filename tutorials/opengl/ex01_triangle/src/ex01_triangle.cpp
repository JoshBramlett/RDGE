#include <rdge/types.hpp>
#include <rdge/surface.hpp>
#include <rdge/application.hpp>
#include <rdge/glwindow.hpp>
#include <rdge/random.hpp>
#include <rdge/font.hpp>
#include <rdge/graphics/buffers/vertex_array.hpp>
#include <rdge/graphics/buffers/vertex_buffer.hpp>
#include <rdge/graphics/buffers/index_buffer.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/renderer2d.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/label.hpp>
#include <rdge/graphics/layers/layer2d.hpp>
#include <rdge/graphics/layers/group.hpp>
#include <rdge/graphics/gltexture.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/vec4.hpp>
#include <rdge/math/mat4.hpp>
#include <rdge/util/io.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/system_info.hpp>

#include <SDL.h>
#include <GL/glew.h>

#include <iostream>
#include <vector>
#include <memory>

using namespace RDGE::Graphics;
using namespace RDGE::Math;

enum class Josh
{
    Harmie,
    Calder
};

//const char*
std::string
JoshString (Josh josh)
{
    switch (josh) {
#define CASE(X) case X: return #X;
    CASE(Josh::Harmie)
    CASE(Josh::Calder)
#undef CASE
        default:
        return "Unknown";
    }
}

int main ()
{
    std::cout << JoshString(Josh::Harmie) << std::endl;

    try
    {
        std::cout << "Running ex01_triangle" << std::endl;

        // 1 - initialize SDL
        RDGE::Application app(SDL_INIT_EVERYTHING, 0, true);
        std::cout << app.SDLVersion() << std::endl;

        // 2 - create window and OpenGL context
        RDGE::GLWindow window (
                               "ex01_triangle",
                               960, 540,
                               false, false,
                               false // vsync
                              );

        auto v = RDGE::Util::read_text_file("basic.vert");
        auto f = RDGE::Util::read_text_file("basic.frag");
        auto shader = std::make_unique<Shader>(v, f);

        auto font = RDGE::Font("OpenSansPX.ttf", 32);

        Layer2D layer(std::move(shader));
        //auto myText = new Label("Josh", 1.0f, 4.0f, std::move(font), RDGE::Color::Red());

        auto texture = std::make_shared<GLTexture>("test.gif");
        auto texture2 = std::make_shared<GLTexture>("test2.gif");

        //auto texture = std::make_shared<GLTexture>("test_map.gif");
        //auto uv1 = Renderable2D::UVCoordinates {
            //RDGE::Math::vec2(0, 1),
            //RDGE::Math::vec2(0, 0),
            //RDGE::Math::vec2(0.5f, 0),
            //RDGE::Math::vec2(0.5f, 1)
        //};
        //auto uv2 = Renderable2D::UVCoordinates {
            //RDGE::Math::vec2(0.5f, 1),
            //RDGE::Math::vec2(0.5f, 0),
            //RDGE::Math::vec2(1, 0),
            //RDGE::Math::vec2(1, 1)
        //};

        RDGE::Random rng;
        for (float y = 0; y < 9.0f; y++)
        {
            for (float x = 0; x < 16.0f; x++)
            {
                auto random = rng.Next(2);
                if (random == 0)
                {
                    layer.AddRenderable(new Sprite(x, y, 0.9f, 0.9f, RDGE::Color::Blue()));
                }
                else if (random == 1)
                {
                    layer.AddRenderable(new Sprite(x, y, 0.9f, 0.9f, texture));
                    //layer.AddRenderable(new Sprite(x, y, 0.9f, 0.9f, texture, uv1));
                }
                else
                {
                    layer.AddRenderable(new Sprite(x, y, 0.9f, 0.9f, texture2));
                    //layer.AddRenderable(new Sprite(x, y, 0.9f, 0.9f, texture, uv2));
                }
            }
        }

        auto myText = new Label("Josh", 1.0f, 4.0f, std::move(font), RDGE::Color::Red());
        layer.AddRenderable(myText);

        //Group* button = new Group(mat4::translation(vec3(1.0f, 0.0f, 0.0f)));
        //button->AddRenderable(new Sprite(0, 0, 5.0f, 2.0f, RDGE::Color::Blue()));
        //button->AddRenderable(new Sprite(0.5f, 0.5f, 3.0f, 1.0f, RDGE::Color::Red()));
        //layer.AddRenderable(button);

        bool running = true;
        SDL_Event event;
        while (running)
        {
            int x, y;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                    break;
                }
                else if (event.type == SDL_KEYDOWN)
                {
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_j:
                        myText->SetText("Josh Two");
                        break;
                    }
                }
                else if (event.type == SDL_MOUSEMOTION)
                {
                    SDL_GetMouseState(&x, &y);
                }
            }

            window.Clear();

            auto lp = vec2(
                           static_cast<float>(x * 16.0f / 960.0f),
                           static_cast<float>(9.0f - y * 9.0f / 540.0f)
                          );
            auto layer_shader = layer.GetShader();
            layer_shader->Enable();
            layer_shader->SetUniform2f("light_pos", lp);

            layer.Render();

            window.Present();
            //printf("%f fps\n", window.FrameRate());
        }
    }
    catch (const RDGE::SDLException& ex)
    {
        std::cout << "SDLException Raised!" << std::endl;
        std::cout << "    " << ex.what() << std::endl;
        std::cout << "    " << ex.SDLFunction() << std::endl;
        std::cout << "    " << ex.SDLError() << std::endl;
    }
    catch (const RDGE::GLException& ex)
    {
        std::cout << "GLException Raised!" << std::endl;
        std::cout << "    " << ex.what() << std::endl;
        std::cout << "    " << ex.GLFunction() << std::endl;
        std::cout << "    " << ex.GLErrorCodeString() << std::endl;
    }
    catch (const RDGE::Exception& ex)
    {
        std::cout << "Exception Raised!" << std::endl;
        std::cout << "    " << ex.what() << std::endl;
        std::cout << "    " << ex.File() << std::endl;
        std::cout << "    " << ex.FileName() << ":"
                  << ex.Line() << " [" << ex.Function() << "]" << std::endl;
    }
    catch (...)
    {
        std::cout << "Unknown exception" << std::endl;
    }

    return 0;
}
