//! \headerfile <rdge/util/strings.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 02/27/2017

#pragma once

#include <rdge/core.hpp>

#include <string>
#include <algorithm>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

inline std::string to_lower (const std::string& value)
{
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);

    return result;
}

inline std::string to_upper (const std::string& value)
{
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);

    return result;
}

} // namespace rdge
