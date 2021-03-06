//! \headerfile <rdge/util/strings.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 02/27/2017

#pragma once

#include <rdge/core.hpp>

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

inline std::string
to_lower (const std::string& value)
{
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);

    return result;
}

inline std::string
to_upper (const std::string& value)
{
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);

    return result;
}

inline void
ltrim (std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

inline void
rtrim (std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

inline void
trim (std::string &s)
{
    ltrim(s);
    rtrim(s);
}

inline std::vector<std::string>
split (const std::string& s, char delim = ' ')
{
    std::vector<std::string> result;
    for (size_t p = 0, q = 0; p != s.npos; p = q)
    {
        p = s.find_first_not_of(delim, p);
        result.emplace_back(s.substr(p, (q = s.find(delim, p + 1)) - p));
    }

    return result;
}

inline bool
starts_with (const std::string& source, const std::string& prefix)
{
    return source.rfind(prefix, 0) == 0;
}

inline bool
ends_with (const std::string& source, const std::string& suffix)
{
    int64 pos = source.length() - suffix.length();
    if (pos < 0)
    {
        return false;
    }

    return source.rfind(suffix, static_cast<size_t>(pos)) == static_cast<size_t>(pos);
}

inline bool
contains (const std::string& source, const std::string& test)
{
    return source.find(test) != std::string::npos;
}

inline void
remove_all (std::string& source, char c)
{
    source.erase(std::remove(source.begin(), source.end(), c), source.end());
}

inline std::string
basename (const std::string& filepath)
{
    auto pos = filepath.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        return filepath.substr(pos + 1);
    }

    return filepath;
}

inline std::string
dirname (const std::string& filepath)
{
    auto pos = filepath.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        return filepath.substr(0, pos);
    }

    return filepath;
}

inline std::string
remove_extension (const std::string& filename)
{
    auto pos = filename.find_last_of(".");
    if (pos != std::string::npos)
    {
        return filename.substr(0, pos);
    }

    return filename;
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
