//! \headerfile <rdge/util/system_info.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 03/15/2016
//! \bug

#pragma once

#include <rdge/types.hpp>

#include <SDL.h>

#include <sstream>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Util {

inline std::string
PrintRendererDriverInfo (void)
{
    std::stringstream ss;
    RDGE::Int32 count = SDL_GetNumRenderDrivers();

    ss << "========== RendererDriverInfo ==========" << std::endl
       << "Drivers found: " << count << std::endl
       << std::endl;

    SDL_RendererInfo info;
    for (RDGE::Int32 i = 0; i < count; ++i)
    {
        if (UNLIKELY(SDL_GetRenderDriverInfo(0, &info) != 0))
        {
            ss << "SDL_GetRenderDriverInfo[" << i << "]" << std::endl
               << "error=" << SDL_GetError() << std::endl;

            continue;
        }

        ss << "SDL_GetRenderDriverInfo[" << i << "]" << std::endl
           << "name=" << info.name << std::endl
           << "max_texture_width=" << info.max_texture_width << std::endl
           << "max_texture_height=" << info.max_texture_height << std::endl
           << "SDL_RENDERER_SOFTWARE=" << (info.flags&SDL_RENDERER_SOFTWARE) << std::endl
           << "SDL_RENDERER_ACCELERATED=" << (info.flags&SDL_RENDERER_ACCELERATED) << std::endl
           << "SDL_RENDERER_PRESENTVSYNC=" << (info.flags&SDL_RENDERER_PRESENTVSYNC) << std::endl
           << "SDL_RENDERER_TARGETTEXTURE=" << (info.flags&SDL_RENDERER_TARGETTEXTURE) << std::endl;
    }

    return ss.str();
}

} // namespace Util
} // namespace RDGE
