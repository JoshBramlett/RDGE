#include <rdge/graphics/isprite.hpp>

#include <SDL_assert.h>

namespace rdge {

math::vec2&
tex_coords::operator[] (uint32 index) noexcept
{
    SDL_assert(index < 4);
    return (&top_left)[index];
}

const math::vec2&
tex_coords::operator[] (uint32 index) const noexcept
{
    SDL_assert(index < 4);
    return (&top_left)[index];
}

} // namespace rdge
