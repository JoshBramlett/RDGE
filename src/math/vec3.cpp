#include <rdge/math/vec3.hpp>

#include <sstream>
#include <iomanip>

namespace rdge {
namespace math {

std::ostream& operator<< (std::ostream& os, const vec3& value)
{
    return os << rdge::to_string(value);
}

} // namespace math
} // namespace rdge

namespace rdge {

std::string
to_string (const math::vec3& value)
{
    std::ostringstream ss;
    ss << "["
       << std::fixed << std::setprecision(6)
       << value.x << ", "
       << value.y << ", "
       << value.z
       << "]";

    return ss.str();
}

} // namespace rdge
