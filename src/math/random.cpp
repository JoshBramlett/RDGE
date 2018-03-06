#include <rdge/math/random.hpp>

#include <limits>
#include <random>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace math {

namespace {
    std::random_device s_rd;
    std::mt19937 s_mt(s_rd());

    float ONE_INC = 1.f + std::numeric_limits<float>::min();
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
    std::uniform_real_distribution<float> dist(0.f, 1.f);
    return dist(s_mt);
}

vec2
Random::Normal (void) const
{
    std::uniform_real_distribution<float> x(-1.f, ONE_INC);
    std::uniform_real_distribution<float> y(-1.f, ONE_INC);

    return vec2(x(s_mt), y(s_mt));
}

} // namespace math
} // namespace rdge
