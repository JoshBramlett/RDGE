//! \headerfile <rdge/debug/sdl_dumper.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/22/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/graphics/color.hpp>
#include <rdge/util/compiler.hpp>

#include <SDL.h>

#include <string>
#include <sstream>
#include <iomanip>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace debug {

inline std::string
WindowEventString (const SDL_WindowEvent& event)
{
    switch (event.event)
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
        break;
    }

    return "Unknown";
}

inline std::string
DumpPixelFormatEnum (uint32 format)
{
    std::ostringstream ss;
    ss << "\n\nSDL_PixelFormatEnum"
       << "\n  PixelFormatName.......... " << SDL_GetPixelFormatName(format)
       << "\n  SDL_PIXELTYPE............ " << SDL_PIXELTYPE(format)
       << "\n  SDL_PIXELORDER........... " << SDL_PIXELORDER(format)
       << "\n  SDL_PIXELLAYOUT.......... " << SDL_PIXELLAYOUT(format)
       << "\n  SDL_BITSPERPIXEL......... " << SDL_BITSPERPIXEL(format)
       << "\n  SDL_BYTESPERPIXEL........ " << SDL_BYTESPERPIXEL(format)
       << "\n  SDL_ISPIXELFORMAT_INDEXED " << SDL_ISPIXELFORMAT_INDEXED(format)
       << "\n  SDL_ISPIXELFORMAT_ALPHA.. " << SDL_ISPIXELFORMAT_ALPHA(format)
       << "\n  SDL_ISPIXELFORMAT_FOURCC. " << SDL_ISPIXELFORMAT_FOURCC(format);

    return ss.str();
}

inline std::string
DumpPixelFormat (SDL_PixelFormat* pixel_format)
{
    std::ostringstream ss;
    ss << "========== SDL_PixelFormat =========="
       << DumpPixelFormatEnum(pixel_format->format);

    ss << "\n\nSDL_Palette";
    if (pixel_format->palette == nullptr)
    {
        ss << "\n  None";
    }
    else
    {
        for (int i = 0; i < pixel_format->palette->ncolors; ++i)
        {
            color c(pixel_format->palette->colors[i]);
            ss << "\n  " << c;
        }
    }

    ss << std::hex << std::uppercase << std::setfill('0')
       << "\nrmask= " << std::setw(8) << pixel_format->Rmask
       << "\ngmask= " << std::setw(8) << pixel_format->Gmask
       << "\nbmask= " << std::setw(8) << pixel_format->Bmask
       << "\namask= " << std::setw(8) << pixel_format->Amask
       << "\n";

    return ss.str();
}

inline std::string
DumpRendererDriverInfo (void)
{
    int count = SDL_GetNumRenderDrivers();

    std::ostringstream ss;
    ss << "========== RendererDriverInfo ==========\n"
       << "\nDrivers found: " << count;

    SDL_RendererInfo info;
    for (int i = 0; i < count; ++i)
    {
        if (RDGE_UNLIKELY(SDL_GetRenderDriverInfo(0, &info) != 0))
        {
            ss << "\nSDL_GetRenderDriverInfo[" << i << "]"
               << "\n  error=" << SDL_GetError();

            continue;
        }

        ss << "\nSDL_GetRenderDriverInfo[" << i << "]"
           << "\nname=" << info.name
           << "\nmax_texture_width=" << info.max_texture_width
           << "\nmax_texture_height=" << info.max_texture_height
           << "\n*** FLAGS ***"
           << "\nSDL_RENDERER_SOFTWARE..... " << (info.flags&SDL_RENDERER_SOFTWARE)
           << "\nSDL_RENDERER_ACCELERATED.. " << (info.flags&SDL_RENDERER_ACCELERATED)
           << "\nSDL_RENDERER_PRESENTVSYNC. " << (info.flags&SDL_RENDERER_PRESENTVSYNC)
           << "\nSDL_RENDERER_TARGETTEXTURE " << (info.flags&SDL_RENDERER_TARGETTEXTURE)
           << "\n*** TEXTURE FORMATS ***\n";

        for (uint i = 0; i < info.num_texture_formats; ++i)
        {
            ss << DumpPixelFormatEnum(info.texture_formats[i]);
        }
    }

    return ss.str();
}

inline std::string
DumpSDLOpenGLAttributes (void)
{
    int context_major, context_minor, profile;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &context_major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &context_minor);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);

    int share_context, release_behavior, flags;
    SDL_GL_GetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, &share_context);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_RELEASE_BEHAVIOR, &release_behavior);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &flags);

    std::ostringstream ss;
    ss << "========== SDLOpenGLAttributes =========="
       << "\n\n*** CONTEXT ***"
       << "\nSDL_GL_CONTEXT_MAJOR_VERSION..... " << context_major
       << "\nSDL_GL_CONTEXT_MINOR_VERSION..... " << context_minor
       << "\nSDL_GL_CONTEXT_PROFILE_MASK...... " << profile
       << "\nSDL_GL_SHARE_WITH_CURRENT_CONTEXT " << share_context
       << "\nSDL_GL_CONTEXT_RELEASE_BEHAVIOR.. " << release_behavior
       << "\nSDL_GL_CONTEXT_FLAGS"
       << "\n  GL_CONTEXT_DEBUG............... " << (flags&SDL_GL_CONTEXT_DEBUG_FLAG)
       << "\n  GL_CONTEXT_FORWARD_COMPATIBLE.. " << (flags&SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG)
       << "\n  GL_CONTEXT_ROBUST_ACCESS....... " << (flags&SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG)
       << "\n  GL_CONTEXT_RESET_ISOLATION..... " << (flags&SDL_GL_CONTEXT_RESET_ISOLATION_FLAG);

    int buffer_size, double_buffer, srgb_capable, depth_size, stencil_size;
    SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &buffer_size);
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &double_buffer);
    SDL_GL_GetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, &srgb_capable);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth_size);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil_size);

    ss << "\n\n*** FRAME BUFFER ***"
       << "\nSDL_GL_BUFFER_SIZE............... " << buffer_size
       << "\nSDL_GL_DOUBLEBUFFER.............. " << double_buffer
       << "\nSDL_GL_FRAMEBUFFER_SRGB_CAPABLE.. " << srgb_capable
       << "\n[Depth buffer]"
       << "\n  SDL_GL_DEPTH_SIZE.............. " << depth_size
       << "\n[Stencil buffer]"
       << "\n  SDL_GL_STENCIL_SIZE............ " << stencil_size;

    int r, g, b, a;
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &r);
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &g);
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &b);
    SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &a);

    ss << "\n[Color buffer]"
       << "\n  SDL_GL_RED_SIZE................ " << r
       << "\n  SDL_GL_GREEN_SIZE.............. " << g
       << "\n  SDL_GL_BLUE_SIZE............... " << b
       << "\n  SDL_GL_ALPHA_SIZE.............. " << a;

    int accum_r, accum_g, accum_b, accum_a;
    SDL_GL_GetAttribute(SDL_GL_ACCUM_RED_SIZE, &accum_r);
    SDL_GL_GetAttribute(SDL_GL_ACCUM_GREEN_SIZE, &accum_g);
    SDL_GL_GetAttribute(SDL_GL_ACCUM_BLUE_SIZE, &accum_b);
    SDL_GL_GetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, &accum_a);

    ss << "\n[Accumulation buffer]"
       << "\n  SDL_GL_ACCUM_RED_SIZE.......... " << accum_r
       << "\n  SDL_GL_ACCUM_GREEN_SIZE........ " << accum_g
       << "\n  SDL_GL_ACCUM_BLUE_SIZE......... " << accum_b
       << "\n  SDL_GL_ACCUM_ALPHA_SIZE........ " << accum_a;

    int ms_buffers, ms_samples;
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &ms_buffers);
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &ms_samples);

    ss << "\n\n*** MULTI-SAMPLING ***"
       << "\nSDL_GL_MULTISAMPLEBUFFERS........ " << ms_buffers
       << "\nSDL_GL_MULTISAMPLESAMPLES........ " << ms_samples;

    int stereo, accelerated_visual;
    SDL_GL_GetAttribute(SDL_GL_STEREO, &stereo);
    SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &accelerated_visual);

    ss << "\n\n*** MISC ***"
       << "\nSDL_GL_STEREO.................... " << stereo
       << "\nSDL_GL_ACCELERATED_VISUAL........ " << accelerated_visual;

    return ss.str();
}

} // namespace debug
} // namespace rdge
