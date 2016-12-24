#include <rdge/graphics/color.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <regex>
#include <iomanip>
#include <sstream>

namespace rdge {

const color color::BLACK (0, 0, 0);
const color color::WHITE (255, 255, 255);
const color color::RED (255, 0, 0);
const color color::GREEN (0, 255, 0);
const color color::BLUE (0, 0, 255);
const color color::YELLOW (255, 255, 0);
const color color::CYAN (0, 255, 255);
const color color::MAGENTA (255, 0, 255);

/* static */ color
color::FromRGB (const std::string& value)
{
    // Preceding '#' is supported.  If there, remove it
    const std::string c = (value.front() == '#')
                            ? value.substr(1)
                            : value;

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
            auto numeric = std::stoul(c, nullptr, 16);
            uint8 r = (numeric >> 16) & 0xFF;
            uint8 g = (numeric >> 8) & 0xFF;
            uint8 b = numeric & 0xFF;

            return color(r, g, b);
        }
        catch (...) { }
    }

    RDGE_THROW("Color parameter could not be parsed");
}

/* static */ color
color::FromRGBA (const std::string& value)
{
    // Preceding '#' is supported.  If there, remove it
    const std::string c = (value.front() == '#')
                            ? value.substr(1)
                            : value;

    // Expression matches 2 character hex values, that appear 4 times
    // (e.g. FF 00 CC AA)
    // Note:  See FromRGB for comments on why regex is required
    std::regex exp("^([0-9a-fA-F]{2}){4}$");
    if (std::regex_match(c, exp))
    {
        try
        {
            auto numeric = std::stoul(c, nullptr, 16);
            uint8 r = (numeric >> 24) & 0xFF;
            uint8 g = (numeric >> 16) & 0xFF;
            uint8 b = (numeric >> 8) & 0xFF;
            uint8 a = numeric & 0xFF;

            return color(r, g, b, a);
        }
        catch (...) { }
    }

    RDGE_THROW("Color parameter could not be parsed");
}

std::ostream& operator<< (std::ostream& os, const color& value)
{
    return os << to_string(value);
}

std::string
to_string (const color& value)
{
    // without static cast output will display the char values
    std::ostringstream ss;
    ss << "#"
       << std::hex << std::uppercase << std::setfill('0')
       << std::setw(2) << static_cast<int>(value.r)
       << std::setw(2) << static_cast<int>(value.g)
       << std::setw(2) << static_cast<int>(value.b)
       << std::setw(2) << static_cast<int>(value.a);

    return ss.str();
}

} // namespace rdge