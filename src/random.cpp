#include <rdge/random.hpp>
#include <rdge/types.hpp>
#include <rdge/util/fp.hpp>

#include <cmath>
#include <random>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {

namespace {
    std::random_device s_rd;
    std::mt19937 s_mt(s_rd());
}

Random::Random (void) { }

Random::~Random (void) { }

RDGE::UInt32
Random::Next (void) const
{
    return Next(0, std::numeric_limits<RDGE::UInt32>::max());
}

RDGE::UInt32
Random::Next (RDGE::UInt32 max) const
{
    return Next(0, max);
}

RDGE::UInt32
Random::Next (RDGE::UInt32 low, RDGE::UInt32 high) const
{
    std::uniform_int_distribution<RDGE::UInt32> dist(low, high);
    return dist(s_mt);
}

double
Random::Sample (void) const
{
    std::uniform_real_distribution<double> dist(0, 1.0);
    return dist(s_mt);
}

} // namespace RDGE
