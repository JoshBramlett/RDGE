#pragma once

#include <rdge/core.hpp>

#include <SDL.h>
#include <GL/glew.h>

#include <exception>

namespace rdge {
namespace tests {

// Hidden window that when initialized creates an OpenGL context.  Testing which
// requires a valid OpenGL context should create a test fixture that includes
// a DummyWindow member variable
class DummyWindow
{
public:
    DummyWindow (void)
    {
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        m_window = SDL_CreateWindow("",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    0,
                                    0,
                                    SDL_WINDOW_HIDDEN|SDL_WINDOW_OPENGL);
        if (!m_window)
        {
            throw std::runtime_error("Failed to create window");
        }

        m_context = SDL_GL_CreateContext(m_window);
        if (!m_context)
        {
            throw std::runtime_error("Failed to create OpenGL context");
        }

        glewExperimental = GL_TRUE;
        GLenum glew_error = glewInit();
        if (glew_error != GLEW_OK)
        {
            throw std::runtime_error("glew failed to init");
        }

        // if glew provided an error, ignore it
        glGetError();
    }

    ~DummyWindow (void) noexcept
    {
        if (m_context)
        {
            SDL_GL_DeleteContext(m_context);
        }

        if (m_context)
        {
            SDL_DestroyWindow(m_window);
        }
    }

    DummyWindow (const DummyWindow&) = delete;
    DummyWindow& operator= (const DummyWindow&) = delete;
    DummyWindow (DummyWindow&&) = delete;
    DummyWindow& operator= (DummyWindow&&) = delete;

private:
    SDL_Window*   m_window = nullptr;
    SDL_GLContext m_context = nullptr;
};

} // namespace tests
} // namespace rdge
