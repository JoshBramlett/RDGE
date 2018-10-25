#include <rdge/graphics/color.hpp>
#include <rdge/util/exception.hpp>

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

std::string
color::to_rgb (void) const noexcept
{
    // without static cast output will display the char values
    std::ostringstream ss;
    ss << "#"
       << std::hex << std::uppercase << std::setfill('0')
       << std::setw(2) << static_cast<int>(this->r)
       << std::setw(2) << static_cast<int>(this->g)
       << std::setw(2) << static_cast<int>(this->b);

    return ss.str();
}

std::string
color::to_rgba (void) const noexcept
{
    // without static cast output will display the char values
    std::ostringstream ss;
    ss << "#"
       << std::hex << std::uppercase << std::setfill('0')
       << std::setw(2) << static_cast<int>(this->r)
       << std::setw(2) << static_cast<int>(this->g)
       << std::setw(2) << static_cast<int>(this->b)
       << std::setw(2) << static_cast<int>(this->a);

    return ss.str();
}

std::string
color::to_argb (void) const noexcept
{
    // without static cast output will display the char values
    std::ostringstream ss;
    ss << "#"
       << std::hex << std::uppercase << std::setfill('0')
       << std::setw(2) << static_cast<int>(this->a)
       << std::setw(2) << static_cast<int>(this->r)
       << std::setw(2) << static_cast<int>(this->g)
       << std::setw(2) << static_cast<int>(this->b);

    return ss.str();
}

/* static */ color
color::from_rgb (const std::string& value)
{
    // Verify length before passing - RGBA should fail here
    if (value.length() != 6 && value.length() != 7)
    {
        RDGE_THROW("Color parameter could not be parsed");
    }

    return color::from_rgba(value);
}

/* static */ color
color::from_rgba (const std::string& value)
{
    // Preceding '#' is supported.  If there, remove it
    const std::string c = (value.front() == '#')
                            ? value.substr(1)
                            : value;

    // Expression matches 2 character hex values that appear 3 or 4 times
    //
    // Regex test is required b/c std::stoi has some loose behavior on the
    // strings it accepts.  With std::stoi you can ensure that all characters
    // have been parsed by matching the index (second parameter) to the string
    // length.  However, std::stoi will also accept '+', '-', and "0x" (for hex)
    // which would leave the incorrect behavior.
    std::regex exp("^([0-9a-fA-F]{2}){3,4}$");
    if (std::regex_match(c, exp))
    {
        try
        {
            if (c.length() == 6)
            {
                auto numeric = std::stoul(c, nullptr, 16);
                uint8 r = (numeric >> 16) & 0xFF;
                uint8 g = (numeric >> 8) & 0xFF;
                uint8 b = numeric & 0xFF;

                return color(r, g, b);
            }
            else
            {
                auto numeric = std::stoul(c, nullptr, 16);
                uint8 r = (numeric >> 24) & 0xFF;
                uint8 g = (numeric >> 16) & 0xFF;
                uint8 b = (numeric >> 8) & 0xFF;
                uint8 a = numeric & 0xFF;

                return color(r, g, b, a);
            }
        }
        catch (...) { }
    }

    RDGE_THROW("Color parameter could not be parsed");
}

/* static */ color
color::from_argb (const std::string& value)
{
    // remove preceding '#'
    const std::string c = (value.front() == '#') ? value.substr(1) : value;

    // Expression matches 2 character hex values that appear 3 or 4 times
    // (e.g. FF 00 CC AA)
    // Note:  See from_rgba for comments on why regex is required
    std::regex exp("^([0-9a-fA-F]{2}){3,4}$");
    if (std::regex_match(c, exp))
    {
        try
        {
            auto numeric = std::stoul(c, nullptr, 16);
            uint8 a = (c.length() == 8) ? (numeric >> 24) & 0xFF : 255;
            uint8 r = (numeric >> 16) & 0xFF;
            uint8 g = (numeric >> 8) & 0xFF;
            uint8 b = numeric & 0xFF;

            return color(r, g, b, a);
        }
        catch (...) { }
    }

    RDGE_THROW("Color parameter could not be parsed");
}

std::ostream& operator<< (std::ostream& os, const color& value)
{
    return os << rdge::to_string(value);
}

std::string
to_string (const color& value)
{
    return value.to_rgba();
}

} // namespace rdge
