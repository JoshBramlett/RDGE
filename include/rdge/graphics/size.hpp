//! \headerfile <rdge/graphics/size.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/22/2016

#pragma once

#include <rdge/core.hpp>
#include <rdge/math/vec2.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \typedef size Unsigned integer vec2_t structure
using size = math::vec2_t<uint32>;

math::vec2
to_ndc (const size& size);

} // namespace rdge
