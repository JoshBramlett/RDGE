//! \headerfile <rdge/system/keyboard.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 02/26/2017

#pragma once

#include <rdge/core.hpp>

#include <SDL.h>

#include <ostream>
#include <string>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

// TODO: The only values supported are those on my Mac.  When I move to
//       full cross platform support I need to add the remainder from here:
//       https://wiki.libsdl.org/SDLKeycodeLookup
//       https://wiki.libsdl.org/SDLScancodeLookup

//! \enum KeyCode
//! \brief Virtual key representation
//! \details Regarded as virtual because it represents the key in the context of
//!          the current keyboard layout.  Because they may not correspond to
//!          physical keys they are better suited for character dependent controls
//!          e.g. Press 'i' for inventory.
//! \note Values map directly to SDL_Keycode
enum class KeyCode : uint32
{
    UNKNOWN    = SDLK_UNKNOWN,
    BACKSPACE  = SDLK_BACKSPACE,
    TAB        = SDLK_TAB,
    RETURN     = SDLK_RETURN,
    ESCAPE     = SDLK_ESCAPE,
    SPACE      = SDLK_SPACE,
    QUOTE      = SDLK_QUOTE,
    COMMA      = SDLK_COMMA,
    MINUS      = SDLK_MINUS,
    PERIOD     = SDLK_PERIOD,
    SLASH      = SDLK_SLASH,
    ZERO       = SDLK_0,
    ONE        = SDLK_1,
    TWO        = SDLK_2,
    THREE      = SDLK_3,
    FOUR       = SDLK_4,
    FIVE       = SDLK_5,
    SIX        = SDLK_6,
    SEVEN      = SDLK_7,
    EIGHT      = SDLK_8,
    NINE       = SDLK_9,
    SEMICOLON  = SDLK_SEMICOLON,
    EQUALS     = SDLK_EQUALS,
    BACKQUOTE  = SDLK_BACKQUOTE,
    A          = SDLK_a,
    B          = SDLK_b,
    C          = SDLK_c,
    D          = SDLK_d,
    E          = SDLK_e,
    F          = SDLK_f,
    G          = SDLK_g,
    H          = SDLK_h,
    I          = SDLK_i,
    J          = SDLK_j,
    K          = SDLK_k,
    L          = SDLK_l,
    M          = SDLK_m,
    N          = SDLK_n,
    O          = SDLK_o,
    P          = SDLK_p,
    Q          = SDLK_q,
    R          = SDLK_r,
    S          = SDLK_s,
    T          = SDLK_t,
    U          = SDLK_u,
    V          = SDLK_v,
    W          = SDLK_w,
    X          = SDLK_x,
    Y          = SDLK_y,
    Z          = SDLK_z,
    CAPSLOCK   = SDLK_CAPSLOCK,
    F1         = SDLK_F1,
    F2         = SDLK_F2,
    F3         = SDLK_F3,
    F4         = SDLK_F4,
    F5         = SDLK_F5,
    F6         = SDLK_F6,
    F7         = SDLK_F7,
    F8         = SDLK_F8,
    F9         = SDLK_F9,
    F10        = SDLK_F10,
    F11        = SDLK_F11,
    F12        = SDLK_F12,
    RIGHT      = SDLK_RIGHT,
    LEFT       = SDLK_LEFT,
    DOWN       = SDLK_DOWN,
    UP         = SDLK_UP,
    LEFTCTRL   = SDLK_LCTRL,
    LEFTSHIFT  = SDLK_LSHIFT,
    LEFTALT    = SDLK_LALT,
    LEFTGUI    = SDLK_LGUI,
    RIGHTCTRL  = SDLK_RCTRL,
    RIGHTSHIFT = SDLK_RSHIFT,
    RIGHTALT   = SDLK_RALT,
    RIGHTGUI   = SDLK_RGUI
};

//! \enum ScanCode
//! \brief Physical key representation
//! \details Maps to physical keys on the keyboard.  Their use is better suited for
//!          layout dependent controls.  e.g. WASD.
//! \note Values map directly to SDL_Scancode
enum class ScanCode : uint32
{
    UNKNOWN    = SDL_SCANCODE_UNKNOWN,
    BACKSPACE  = SDL_SCANCODE_BACKSPACE,
    TAB        = SDL_SCANCODE_TAB,
    RETURN     = SDL_SCANCODE_RETURN,
    ESCAPE     = SDL_SCANCODE_ESCAPE,
    SPACE      = SDL_SCANCODE_SPACE,
    QUOTE      = SDL_SCANCODE_APOSTROPHE,
    COMMA      = SDL_SCANCODE_COMMA,
    MINUS      = SDL_SCANCODE_MINUS,
    PERIOD     = SDL_SCANCODE_PERIOD,
    SLASH      = SDL_SCANCODE_SLASH,
    ZERO       = SDL_SCANCODE_0,
    ONE        = SDL_SCANCODE_1,
    TWO        = SDL_SCANCODE_2,
    THREE      = SDL_SCANCODE_3,
    FOUR       = SDL_SCANCODE_4,
    FIVE       = SDL_SCANCODE_5,
    SIX        = SDL_SCANCODE_6,
    SEVEN      = SDL_SCANCODE_7,
    EIGHT      = SDL_SCANCODE_8,
    NINE       = SDL_SCANCODE_9,
    SEMICOLON  = SDL_SCANCODE_SEMICOLON,
    EQUALS     = SDL_SCANCODE_EQUALS,
    BACKQUOTE  = SDL_SCANCODE_GRAVE,
    A          = SDL_SCANCODE_A,
    B          = SDL_SCANCODE_B,
    C          = SDL_SCANCODE_C,
    D          = SDL_SCANCODE_D,
    E          = SDL_SCANCODE_E,
    F          = SDL_SCANCODE_F,
    G          = SDL_SCANCODE_G,
    H          = SDL_SCANCODE_H,
    I          = SDL_SCANCODE_I,
    J          = SDL_SCANCODE_J,
    K          = SDL_SCANCODE_K,
    L          = SDL_SCANCODE_L,
    M          = SDL_SCANCODE_M,
    N          = SDL_SCANCODE_N,
    O          = SDL_SCANCODE_O,
    P          = SDL_SCANCODE_P,
    Q          = SDL_SCANCODE_Q,
    R          = SDL_SCANCODE_R,
    S          = SDL_SCANCODE_S,
    T          = SDL_SCANCODE_T,
    U          = SDL_SCANCODE_U,
    V          = SDL_SCANCODE_V,
    W          = SDL_SCANCODE_W,
    X          = SDL_SCANCODE_X,
    Y          = SDL_SCANCODE_Y,
    Z          = SDL_SCANCODE_Z,
    CAPSLOCK   = SDL_SCANCODE_CAPSLOCK,
    F1         = SDL_SCANCODE_F1,
    F2         = SDL_SCANCODE_F2,
    F3         = SDL_SCANCODE_F3,
    F4         = SDL_SCANCODE_F4,
    F5         = SDL_SCANCODE_F5,
    F6         = SDL_SCANCODE_F6,
    F7         = SDL_SCANCODE_F7,
    F8         = SDL_SCANCODE_F8,
    F9         = SDL_SCANCODE_F9,
    F10        = SDL_SCANCODE_F10,
    F11        = SDL_SCANCODE_F11,
    F12        = SDL_SCANCODE_F12,
    RIGHT      = SDL_SCANCODE_RIGHT,
    LEFT       = SDL_SCANCODE_LEFT,
    DOWN       = SDL_SCANCODE_DOWN,
    UP         = SDL_SCANCODE_UP,
    LEFTCTRL   = SDL_SCANCODE_LCTRL,
    LEFTSHIFT  = SDL_SCANCODE_LSHIFT,
    LEFTALT    = SDL_SCANCODE_LALT,
    LEFTGUI    = SDL_SCANCODE_LGUI,
    RIGHTCTRL  = SDL_SCANCODE_RCTRL,
    RIGHTSHIFT = SDL_SCANCODE_RSHIFT,
    RIGHTALT   = SDL_SCANCODE_RALT,
    RIGHTGUI   = SDL_SCANCODE_RGUI
};

//!@{
//! \brief Check if provided key is pressed
//! \param [in] key Code to query
//! \returns True if key is pressed, false otherwise
bool IsKeyPressed (KeyCode key) noexcept;
bool IsKeyPressed (ScanCode key) noexcept;
//!@}

//! \brief KeyCode stream output operator
//! \param [in] os Output stream
//! \param [in] key KeyCode enum
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, KeyCode key);

//! \brief ScanCode stream output operator
//! \param [in] os Output stream
//! \param [in] key ScanCode enum
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, ScanCode key);

} // namespace rdge
