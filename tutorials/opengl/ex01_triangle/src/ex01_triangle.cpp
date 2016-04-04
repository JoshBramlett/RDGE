#include <iostream>

#include <rdge/types.hpp>
#include <rdge/application.hpp>
#include <rdge/glwindow.hpp>
#include <rdge/graphics/buffers/vertex_array.hpp>
#include <rdge/graphics/buffers/vertex_buffer.hpp>
#include <rdge/graphics/buffers/index_buffer.hpp>
#include <rdge/graphics/shader.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/math/vec3.hpp>
#include <rdge/math/vec4.hpp>
#include <rdge/math/mat4.hpp>
#include <rdge/util/io.hpp>
#include <rdge/util/exception.hpp>

#include <SDL.h>
#include <GL/glew.h>


using namespace RDGE::Graphics;
using namespace RDGE::Math;

int main ()
{
    GLfloat vertices[] =
    {
        0, 0, 0,
        0, 3, 0,
        8, 3, 0,
        8, 0, 0
    };

    GLushort indices[] =
    {
        0, 1, 2,
        2, 3, 0
    };

    GLfloat colorsA[] =
    {
        1, 0, 1, 1,
        1, 0, 1, 1,
        1, 0, 1, 1,
        1, 0, 1, 1
    };

    GLfloat colorsB[] =
    {
        0.2f, 0.3f, 0.8f, 1,
        0.2f, 0.3f, 0.8f, 1,
        0.2f, 0.3f, 0.8f, 1,
        0.2f, 0.3f, 0.8f, 1
    };

    try
    {
        std::cout << "Running ex01_triangle" << std::endl;
        // 1 - initialize SDL
        RDGE::Application app(SDL_INIT_EVERYTHING, 0, true);

        // 2 - create window and OpenGL context
        RDGE::GLWindow window (
                               "ex01_triangle",
                               960, 540,
                               false, false, true // uses vsync
                              );

        VertexArray sprite1;
        VertexArray sprite2;
        IndexBuffer ibo(indices, 6);

        sprite1.AddBuffer(new VertexBuffer(vertices, 4 * 3, 3), 0);
        sprite1.AddBuffer(new VertexBuffer(colorsA, 4 * 4, 4), 1);

        sprite2.AddBuffer(new VertexBuffer(vertices, 4 * 3, 3), 0);
        sprite2.AddBuffer(new VertexBuffer(colorsB, 4 * 4, 4), 1);

        mat4 ortho = mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f);

        Shader shader("basic.vert", "basic.frag");
        shader.Enable();
	    shader.SetUniformMat4("pr_matrix", ortho);
	    shader.SetUniformMat4("ml_matrix", mat4::translate(vec3(4, 3, 0)));

	    shader.SetUniform2f("light_pos", vec2(4.0f, 1.5f));
	    shader.SetUniform4f("colour", vec4(0.2f, 0.3f, 0.8f, 1.0f));

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
            shader.SetUniform2f("light_pos", lp);

            sprite1.Bind();
            ibo.Bind();
            shader.SetUniformMat4("ml_matrix", mat4::translate(vec3(4, 3, 0)));
            glDrawElements(GL_TRIANGLES, ibo.Count(), GL_UNSIGNED_SHORT, 0);
            ibo.Unbind();
            sprite1.Unbind();

            sprite2.Bind();
            ibo.Bind();
            shader.SetUniformMat4("ml_matrix", mat4::translate(vec3(0, 0, 0)));
            glDrawElements(GL_TRIANGLES, ibo.Count(), GL_UNSIGNED_SHORT, 0);
            ibo.Unbind();
            sprite2.Unbind();

            window.Present();
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
