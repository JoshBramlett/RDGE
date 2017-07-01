//! \headerfile <rdge/math/random.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 01/08/2016

#pragma once

#include <rdge/core.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace rdge {
namespace math {

//! \class Random
//! \brief Simple class used for random number generation.
//! \details Due to limited compiler support math error handling is
//!          not available so use of this class comes with the caveat
//!          that ranges at or outside overflow/underflow will cause
//!          undefined behavior.
//! \see https://llvm.org/bugs/show_bug.cgi?id=10409
//! \see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=37838
class Random
{
public:
    //! \brief Random default ctor
    Random (void) = default;

    //! \brief Generates a random integer within [0, MAX_INT]
    uint32 Next (void) const;

    //! \brief Generates a random integer within [0, max]
    //! \param [in] max Maximum inclusive value
    uint32 Next (uint32 max) const;

    //! \brief Generates a random integer within [low, high]
    //! \param [in] low Low end inclusive range
    //! \param [in] high High end inclusive range
    uint32 Next (uint32 low, uint32 high) const;

    //! \brief Generates a random normalized float within [0, 1)
    float Sample (void) const;
};

} // namespace math
} // namespace rdge
