#include <rdge/types.hpp>
#include <rdge/surface.hpp>
#include <rdge/application.hpp>
#include <rdge/glwindow.hpp>
#include <rdge/graphics/buffers/vertex_array.hpp>
#include <rdge/graphics/buffers/vertex_buffer.hpp>
#include <rdge/graphics/buffers/index_buffer.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/renderer2d.hpp>
#include <rdge/graphics/sprite.hpp>
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

        //std::cout << RDGE::Util::PrintRendererDriverInfo() << std::endl;

        // 2 - create window and OpenGL context
        RDGE::GLWindow window (
                               "ex01_triangle",
                               960, 540,
                               false, false, false // uses vsync
                              );

        auto v = RDGE::Util::read_text_file("basic.vert");
        auto f = RDGE::Util::read_text_file("basic.frag");
        auto shader = std::make_unique<Shader>(v, f);

        shader->Enable();
		shader->SetUniform2f("light_pos", vec2(4.0f, 1.5f));
        shader->SetUniform1i("tex", 0);

        Layer2D layer(std::move(shader));
        for (float y = 0; y < 9.0f; y++)
        {
            for (float x = 0; x < 16.0f; x++)
            {
                layer.AddRenderable(new Sprite(x, y, 0.9f, 0.9f, RDGE::Color::Blue()));
            }
        }

        //Group* button = new Group(mat4::translation(vec3(1.0f, 0.0f, 0.0f)));
        //button->AddRenderable(new Sprite(0, 0, 5.0f, 2.0f, RDGE::Color::Blue()));
        //button->AddRenderable(new Sprite(0.5f, 0.5f, 3.0f, 1.0f, RDGE::Color::Red()));
        //layer.AddRenderable(button);

        //glActiveTexture(GL_TEXTURE0);
        GLTexture texture("test.gif", 0);
        texture.Bind();


            //auto ts = layer.GetShader();
            //ts->Enable();
            //ts->SetUniform1i("tex", 0);
            //ts->SetUniformMat4("pr_matrix", mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f));


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
