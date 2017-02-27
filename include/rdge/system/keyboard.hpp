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

//! \enum KeyCode
//! \brief SDL defined virtual key representation
//! \details Direct mapping to SDL_Keycode, provided for abstraction and
//!          ease of discovery.
enum class KeyCode : uint32
{
    // TODO: The only KeyCode values supported are those on my Mac.  When I move to
    //       full cross platform support I need to add the remainder from here:
    //       https://wiki.libsdl.org/SDLKeycodeLookup
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

//! \brief Check if provided key is pressed
//! \param [in] key Key code to query
//! \returns True if key is pressed, false otherwise
bool IsKeyPressed (KeyCode key) noexcept;

//! \brief KeyCode stream output operator
//! \param [in] os Output stream
//! \param [in] key KeyCode enum
//! \returns Output stream
std::ostream& operator<< (std::ostream& os, KeyCode key);

} // namespace rdge
