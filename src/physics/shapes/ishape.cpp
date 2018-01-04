#include <rdge/physics/shapes/ishape.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/physics/shapes/polygon.hpp>

#include <sstream>
#include <cstring>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {
namespace physics {

std::ostream&
operator<< (std::ostream& os, const mass_data& value)
{
    return os << "[ centroid=" << value.centroid
              << " mass=" << value.mass
              << " mmoi=" << value.mmoi << " ]";
}

std::ostream&
operator<< (std::ostream& os, ShapeType value)
{
    return os << rdge::to_string(value);
}

} // namespace physics

std::string
to_string (physics::ShapeType value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(physics::ShapeType::INVALID)
        CASE(physics::ShapeType::CIRCLE)
        CASE(physics::ShapeType::POLYGON)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

bool
try_parse (const std::string& test, physics::ShapeType& out)
{
    std::string s = rdge::to_lower(test);
    if      (s == "invalid") { out = physics::ShapeType::INVALID; return true; }
    else if (s == "circle")  { out = physics::ShapeType::CIRCLE;  return true; }
    else if (s == "polygon") { out = physics::ShapeType::POLYGON; return true; }

    return false;
}

} // namespace rdge
