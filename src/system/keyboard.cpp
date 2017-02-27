#include <rdge/system/keyboard.hpp>

namespace rdge {

bool
IsKeyPressed (KeyCode key) noexcept
{
    auto scancode = SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(key));

    const uint8* state = SDL_GetKeyboardState(nullptr);
    return state[scancode] == 1;
}

std::ostream& operator<< (std::ostream& os, KeyCode key)
{
    return os << SDL_GetKeyName(static_cast<SDL_Keycode>(key));
}

} // namespace rdge
