#include <rdge/color.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <iomanip>
#include <sstream>

namespace RDGE {

/* static */ RDGE::Color
Color::FromRGB (const std::string& color)
{
    std::string c = (color.front() == '#')
                        ? color.substr(1)
                        : color;

    if (c.size() == 6)
    {
        try
        {
            RDGE::UInt32 numeric = std::stoi(c, nullptr, 16);
            RDGE::UInt8 r = (numeric >> 16) & 0xFF;
            RDGE::UInt8 g = (numeric >> 8) & 0xFF;
            RDGE::UInt8 b = numeric & 0xFF;

            return Color(r, g, b, 0);
        }
        catch (...) { }
    }

    RDGE_THROW("Color parameter could not be parsed");
}

/* static */ RDGE::Color
Color::FromRGBA (const std::string& color)
{
    std::string c = (color.front() == '#')
                        ? color.substr(1)
                        : color;

    if (c.size() == 8)
    {
        try
        {
            RDGE::UInt64 numeric = std::stoul(c, nullptr, 16);
            RDGE::UInt8 r = (numeric >> 24) & 0xFF;
            RDGE::UInt8 g = (numeric >> 16) & 0xFF;
            RDGE::UInt8 b = (numeric >> 8) & 0xFF;
            RDGE::UInt8 a = numeric & 0xFF;

            return Color(r, g, b, a);
        }
        catch (...) { }
    }

    RDGE_THROW("Color parameter could not be parsed");
}

std::ostream& operator<< (std::ostream& os, const Color& color)
{
    // without static cast output will display the char values
    std::stringstream ss;
    ss << "#"
       << std::hex << std::uppercase << std::setfill('0')
       << std::setw(2) << static_cast<RDGE::UInt32>(color.r)
       << std::setw(2) << static_cast<RDGE::UInt32>(color.g)
       << std::setw(2) << static_cast<RDGE::UInt32>(color.b)
       << std::setw(2) << static_cast<RDGE::UInt32>(color.a);

    return os << ss.str();
}

} // namespace RDGE
