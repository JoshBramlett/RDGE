//! \headerfile <rdge/util/strings.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 02/27/2017

#pragma once

#include <rdge/core.hpp>

#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>

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

template <typename T>
inline typename std::enable_if_t<std::is_unsigned<T>::value, std::string>
print_hex (T value)
{
    std::ostringstream ss;
    ss << "0x"
       << std::hex << std::uppercase
       << std::setfill('0') << std::setw(sizeof(value) * 2)
       << value;

    return ss.str();
}

} // namespace rdge
