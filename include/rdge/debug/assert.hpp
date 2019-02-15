//! \headerfile <rdge/debug/assert.hpp>
//! \author Josh Bramlett
//! \version 0.0.11
//! \date 08/24/2018

#pragma once

#include <cassert>
#include <SDL_assert.h>

#define RDGE_ASSERT(x) SDL_assert(x)
#define RDGE_ASSERT_CONSTEXPR(x) assert(x)
