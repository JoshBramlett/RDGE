#include <rdge/system/keyboard.hpp>

namespace rdge {

bool
IsKeyPressed (KeyCode key) noexcept
{
    auto scancode = SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(key));

    const uint8* state = SDL_GetKeyboardState(nullptr);
    return state[scancode] == 1;
}

bool
IsKeyPressed (ScanCode key) noexcept
{
    const uint8* state = SDL_GetKeyboardState(nullptr);
    return state[static_cast<SDL_Scancode>(key)] == 1;
}

std::ostream& operator<< (std::ostream& os, KeyCode key)
{
    return os << SDL_GetKeyName(static_cast<SDL_Keycode>(key));
}

std::ostream& operator<< (std::ostream& os, ScanCode key)
{
    return os << SDL_GetScancodeName(static_cast<SDL_Scancode>(key));
}

} // namespace rdge
