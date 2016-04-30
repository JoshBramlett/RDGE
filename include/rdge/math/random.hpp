//! \headerfile <rdge/math/random.hpp>
//! \author Josh Bramlett
//! \version 0.0.1
//! \date 01/08/2016

#pragma once

#include <rdge/types.hpp>

//! \namespace RDGE Rainbow Drop Game Engine
namespace RDGE {
namespace Math {

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
    //! \brief Random ctor
    Random (void);

    //! \brief Random dtor
    ~Random (void);

    //! \brief Random Copy ctor
    //! \details Non-copyable
    Random (const Random&) = delete;

    //! \brief Random Move ctor
    //! \details Non-moveable
    Random (Random&&) = delete;

    //! \brief Random Copy Assignment Operator
    //! \details Non-copyable
    Random& operator= (const Random&) = delete;

    //! \brief Random Move Assignment Operator
    //! \details Non-moveable
    Random& operator= (Random&&) = delete;

    //! \brief Generates a random integer within [0, MAX_INT]
    //! \details Each random number has a uniform probability of occurring
    //!          and is inclusive of the range provided
    //! \returns Random integer
    RDGE::UInt32 Next (void) const;

    //! \brief Generates a random integer within [0, max]
    //! \details Each random number has a uniform probability of occurring
    //!          and is inclusive of the range provided
    //! \param [in] max Maximum inclusive value
    //! \returns Random integer
    RDGE::UInt32 Next (RDGE::UInt32 max) const;

    //! \brief Generates a random integer within [low, high]
    //! \details Each random number has a uniform probability of occurring
    //!          and is inclusive of the range provided
    //! \param [in] low Low end inclusive range
    //! \param [in] high High end inclusive range
    //! \returns Random integer
    RDGE::UInt32 Next (RDGE::UInt32 low, RDGE::UInt32 high) const;

    //! \brief Generates a random double within [0, 1)
    //! \details Each random number has a uniform probability of occurring
    //!          and is between 0 inclusive and 1 exclusive
    //! \returns Random double
    double Sample (void) const;
};

} // namespace Math
} // namespace RDGE
