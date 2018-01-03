#include <rdge/physics/shapes/ishape.hpp>
#include <rdge/physics/shapes/circle.hpp>
#include <rdge/physics/shapes/polygon.hpp>

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

    return "UNKNOWN";
}

} // namespace rdge
