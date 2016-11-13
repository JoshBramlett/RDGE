#include <rdge/color.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <regex>
#include <iomanip>
#include <sstream>

using namespace rdge;

/* static */ Color
Color::FromRGB (const std::string& color)
{
    // Preceding '#' is supported.  If there, remove it
    const std::string c = (color.front() == '#')
                            ? color.substr(1)
                            : color;

    // Regex test is required b/c std::stoi has some loose behavior on the
    // strings it accepts.  With std::stoi you can ensure that all characters
    // have been parsed by matching the index (second parameter) to the string
    // length.  However, std::stoi will also accept '+', '-', and "0x" (for hex)
    // which would leave the incorrect behavior.
    //
    // Expression matches 2 character hex values, that appear 3 times
    // (e.g. FF 00 CC)
    std::regex exp("^([0-9a-fA-F]{2}){3}$");
    if (std::regex_match(c, exp))
    {
        try
        {
            UInt32 numeric = std::stoi(c, nullptr, 16);
            UInt8 r = (numeric >> 16) & 0xFF;
            UInt8 g = (numeric >> 8) & 0xFF;
            UInt8 b = numeric & 0xFF;

            return Color(r, g, b);
        }
        catch (...) { }
    }

    RDGE_THROW("Color parameter could not be parsed");
}

/* static */ Color
Color::FromRGBA (const std::string& color)
{
    // Preceding '#' is supported.  If there, remove it
    const std::string c = (color.front() == '#')
                            ? color.substr(1)
                            : color;

    // Expression matches 2 character hex values, that appear 4 times
    // (e.g. FF 00 CC AA)
    // Note:  See FromRGB for comments on why regex is required
    std::regex exp("^([0-9a-fA-F]{2}){4}$");
    if (std::regex_match(c, exp))
    {
        try
        {
            UInt64 numeric = std::stoul(c, nullptr, 16);
            UInt8 r = (numeric >> 24) & 0xFF;
            UInt8 g = (numeric >> 16) & 0xFF;
            UInt8 b = (numeric >> 8) & 0xFF;
            UInt8 a = numeric & 0xFF;

            return Color(r, g, b, a);
        }
        catch (...) { }
    }

    RDGE_THROW("Color parameter could not be parsed");
}

std::ostream& operator<< (std::ostream& os, const Color& color)
{
    return os << rdge::to_string(color);
}

std::string
rdge::to_string (const Color& color)
{
    // without static cast output will display the char values
    std::ostringstream ss;
    ss << "#"
       << std::hex << std::uppercase << std::setfill('0')
       << std::setw(2) << static_cast<int>(color.r)
       << std::setw(2) << static_cast<int>(color.g)
       << std::setw(2) << static_cast<int>(color.b)
       << std::setw(2) << static_cast<int>(color.a);

    return ss.str();
}
