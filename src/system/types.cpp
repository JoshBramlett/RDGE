#include <rdge/system/types.hpp>
#include <rdge/util/json.hpp>

#include <sstream>

namespace rdge {

std::string
to_string (const screen_point& value)
{
    std::ostringstream ss;
    ss << "[ " << value.x << ", " << value.y << " ]";

    return ss.str();
}

std::ostream&
operator<< (std::ostream& os, const screen_point& value)
{
    return os << rdge::to_string(value);
}

void
to_json (nlohmann::json& j, const screen_point& value)
{
    j = { { "x", value.x },
          { "y", value.y } };
}

void
from_json (const nlohmann::json& j, screen_point& value)
{
    JSON_VALIDATE_REQUIRED(j, x, is_number_integer);
    JSON_VALIDATE_REQUIRED(j, y, is_number_integer);
    value.x = j["x"].get<decltype(value.x)>();
    value.y = j["y"].get<decltype(value.y)>();
}

std::string
to_string (const screen_rect& value)
{
    std::ostringstream ss;
    ss << "[ x=" << value.x
       << ", y=" << value.y
       << ", w=" << value.w
       << ", h=" << value.h << " ]";

    return ss.str();
}

std::ostream&
operator<< (std::ostream& os, const screen_rect& value)
{
    return os << rdge::to_string(value);
}

void
to_json (nlohmann::json& j, const screen_rect& value)
{
    j = { { "x", value.x },
          { "y", value.y },
          { "w", value.w },
          { "h", value.h } };
}

void
from_json (const nlohmann::json& j, screen_rect& value)
{
    JSON_VALIDATE_REQUIRED(j, x, is_number_integer);
    JSON_VALIDATE_REQUIRED(j, y, is_number_integer);
    JSON_VALIDATE_REQUIRED(j, w, is_number_integer);
    JSON_VALIDATE_REQUIRED(j, h, is_number_integer);
    value.x = j["x"].get<decltype(value.x)>();
    value.y = j["y"].get<decltype(value.y)>();
    value.w = j["w"].get<decltype(value.w)>();
    value.h = j["h"].get<decltype(value.h)>();
}

} // namespace rdge
