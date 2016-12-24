#include <rdge/math/random.hpp>

#include <limits>
#include <random>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace math {

namespace {
    std::random_device s_rd;
    std::mt19937 s_mt(s_rd());
}

Random::Random (void) { }

Random::~Random (void) { }

rdge::uint32
Random::Next (void) const
{
    return Next(0, std::numeric_limits<rdge::uint32>::max());
}

rdge::uint32
Random::Next (rdge::uint32 max) const
{
    return Next(0, max);
}

rdge::uint32
Random::Next (rdge::uint32 low, rdge::uint32 high) const
{
    std::uniform_int_distribution<rdge::uint32> dist(low, high);
    return dist(s_mt);
}

double
Random::Sample (void) const
{
    std::uniform_real_distribution<double> dist(0, 1.0);
    return dist(s_mt);
}

} // namespace math
} // namespace rdge
