#include <rdge/types.hpp>
#include <rdge/config.hpp>
#include <rdge/assets/surface.hpp>
#include <rdge/application.hpp>
#include <rdge/glwindow.hpp>
#include <rdge/assets/font.hpp>
#include <rdge/graphics/buffers/vertex_array.hpp>
#include <rdge/graphics/buffers/vertex_buffer.hpp>
#include <rdge/graphics/buffers/index_buffer.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/graphics/renderer2d.hpp>
#include <rdge/graphics/renderable2d.hpp>
#include <rdge/graphics/sprite.hpp>
#include <rdge/graphics/text.hpp>
#include <rdge/graphics/layers/layer2d.hpp>
#include <rdge/graphics/layers/group.hpp>
#include <rdge/graphics/gltexture.hpp>
#include <rdge/math/random.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/vec4.hpp>
#include <rdge/math/mat4.hpp>
#include <rdge/util/io.hpp>
#include <rdge/util/exception.hpp>
#include <rdge/util/logger.hpp>

#include <SDL.h>
//#include <GL/glew.h>

#include <iostream>
#include <vector>
#include <memory>
#include <typeinfo>

using namespace RDGE::Graphics;
using namespace RDGE::Math;

std::string PrintWindowEvent (SDL_WindowEvent* event)
{

                    switch (event->event)
                    {
#define CASE(X) case X: return #X;
                        CASE(SDL_WINDOWEVENT_SHOWN)
                        CASE(SDL_WINDOWEVENT_HIDDEN)
                        CASE(SDL_WINDOWEVENT_EXPOSED)
                        CASE(SDL_WINDOWEVENT_MOVED)
                        CASE(SDL_WINDOWEVENT_RESIZED)
                        CASE(SDL_WINDOWEVENT_SIZE_CHANGED)
                        CASE(SDL_WINDOWEVENT_MINIMIZED)
                        CASE(SDL_WINDOWEVENT_MAXIMIZED)
                        CASE(SDL_WINDOWEVENT_RESTORED)
                        CASE(SDL_WINDOWEVENT_ENTER)
                        CASE(SDL_WINDOWEVENT_LEAVE)
                        CASE(SDL_WINDOWEVENT_FOCUS_GAINED)
                        CASE(SDL_WINDOWEVENT_FOCUS_LOST)
                        CASE(SDL_WINDOWEVENT_CLOSE)
#undef CASE
                        default:
                            return "Unknown";
                    }
}

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
    //std::cout << JoshString(Josh::Harmie) << std::endl;

    try
    {
        auto config = RDGE::ReadConfigFile("config.json");
        std::cout << "enable_jpg=" << config.enable_jpg << std::endl
                  << "enable_png=" << config.enable_png << std::endl
                  << "enable_tif=" << config.enable_tif << std::endl
                  << "enable_fonts=" << config.enable_fonts << std::endl
                  << "window_title=" << config.window_title << std::endl
                  << "window_icon=" << config.window_icon << std::endl
                  << "target_width=" << config.target_width << std::endl
                  << "target_height=" << config.target_height << std::endl
                  << "fullscreen=" << config.fullscreen << std::endl
                  << "use_vsync=" << config.use_vsync << std::endl
                  << "target_fps=" << config.target_fps << std::endl
                  << "min_log_level=" << config.min_log_level << std::endl;

        // 1 - initialize SDL
        RDGE::Application app(config);

        RDGE::WriteToLogFile(RDGE::LogLevel::Debug, "SDL v" + app.SDLVersion());
        RDGE::WriteToConsole(RDGE::LogLevel::Debug, "Running ex01_triangle");

        // 2 - create window and OpenGL context
        RDGE::GLWindow window (
                               "ex01_triangle",
                               960, 540,
                               false, // fullscreen
                               true, // resizable
                               true  // vsync
                              );

        auto v = RDGE::Util::read_text_file("basic.vert");
        auto f = RDGE::Util::read_text_file("basic.frag");
        auto shader = std::make_unique<Shader>(v, f);

        auto ortho = RDGE::Math::mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f);
        Layer2D layer(std::move(shader), ortho, 1.0f, 500);


        auto rotation_angle = 0.0f;
        auto rotation_vector = vec3(0.0f, 0.0f, 1.0f);

        auto translation = mat4::translation(vec3(6.0f, 2.5f, 0.0f));
        translation *= mat4::rotation(rotation_angle, rotation_vector);
        auto spin_box = std::make_shared<Group>(translation);
        spin_box->AddRenderable(std::make_shared<Sprite>(0.0f, 0.0f, 2.0f, 2.0f, RDGE::Color::Blue()));
        spin_box->AddRenderable(std::make_shared<Sprite>(2.0f, 0.0f, 2.0f, 2.0f, RDGE::Color::Red()));
        spin_box->AddRenderable(std::make_shared<Sprite>(2.0f, 2.0f, 2.0f, 2.0f, RDGE::Color::Green()));
        spin_box->AddRenderable(std::make_shared<Sprite>(0.0f, 2.0f, 2.0f, 2.0f, RDGE::Color::Yellow()));

        auto rotating_sprite = std::make_shared<Sprite>(2.0f, 2.5f, 2.0f, 2.0f, RDGE::Color::Blue());
        layer.AddRenderable(rotating_sprite);

        layer.AddRenderable(spin_box);
/*
        auto font = std::make_shared<RDGE::Assets::Font>("OpenSansPX.ttf", 128);

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

        Random rng;
        for (float y = -9.0f; y < 9.0f; y += 2.0f)
        {
            for (float x = -16.0f; x < 16.0f; x += 2.0f)
            {
                auto random = rng.Next(2);
                if (random == 0)
                {
                    layer.AddRenderable(std::make_shared<Sprite>(x, y, 1.95f, 1.95f, RDGE::Color::Cyan()));
                }
                else if (random == 1)
                {
                    layer.AddRenderable(std::make_shared<Sprite>(x, y, 1.95f, 1.95f, texture));
                    //layer.AddRenderable(new Sprite(x, y, 1.95f, 1.95f, texture, uv1));
                }
                else
                {
                    layer.AddRenderable(std::make_shared<Sprite>(x, y, 1.95f, 1.95f, texture2));
                    //layer.AddRenderable(new Sprite(x, y, 1.95f, 1.95f, texture, uv2));
                }
            }
        }

        auto myText = std::make_shared<Text>("Josh", -14.0f, -1.0f, font, RDGE::Color::White());
        layer.AddRenderable(myText);
*/
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
                    case SDLK_q:
                        {
                        SDL_GetMouseState(&x, &y);
                        auto size = window.Size();
                        auto dsize = window.DrawableSize();
                        auto converted = layer.ConvertScreenCoordinatesToViewport(x, y);
                        std::cout << "x=" << x << " y=" << y << std::endl
                                  << "converted=" << converted << std::endl
                                  << "size=" << size << std::endl
                                  << "dsize=" << dsize << std::endl;
                        }
                        break;
                    case SDLK_a:
                        // aspect ratio: 4x3
                        window.SetSize(1024, 768);
                        break;
                    case SDLK_s:
                        // aspect ratio: 16x10
                        window.SetSize(1280, 800);
                        break;
                    case SDLK_d:
                        // aspect ratio: 16x9
                        window.SetSize(960, 540);
                        break;
                    case SDLK_j:
                        rotation_angle += 1.0f;
                        spin_box->RotateOnCenter(1.0f);
                        //myText->SetText("Josh Two");
                        break;
                    case SDLK_k:
                        //spin_box->SetOpacity(0.5f);
                        //myText->SetColor(RDGE::Color::Red());
                        break;
                    }
                }
                else if (event.type == SDL_MOUSEMOTION)
                {
                    SDL_GetMouseState(&x, &y);
                }
            }

            window.Clear();

            //auto t2 = translation * mat4::translation(vec3(2.0f, 2.0f, 0.0f));
            //t2 *= mat4::rotation(rotation_angle, rotation_vector);
            //t2 *= mat4::translation(vec3(-2.0f, -2.0f, 0.0f));
            //spin_box->SetTransformation(t2);


            //auto lp = vec2(
                           //static_cast<float>(x * 16.0f / 960.0f),
                           //static_cast<float>(9.0f - y * 9.0f / 540.0f)
                          //);
            //auto layer_shader = layer.GetShader();
            //layer_shader->Enable();
            //layer_shader->SetUniformValue("light_pos", lp);

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
