#include <rdge/math/random.hpp>

#include <limits>
#include <random>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace math {

namespace {
    std::random_device s_rd;
    std::mt19937 s_mt(s_rd());
} // anonymous namespace

uint32
Random::Next (void) const
{
    return Next(0, std::numeric_limits<uint32>::max());
}

uint32
Random::Next (uint32 max) const
{
    return Next(0, max);
}

uint32
Random::Next (uint32 low, uint32 high) const
{
    std::uniform_int_distribution<uint32> dist(low, high);
    return dist(s_mt);
}

float
Random::Sample (void) const
{
    std::uniform_real_distribution<float> dist(0, 1.0);
    return dist(s_mt);
}

} // namespace math
} // namespace rdge
