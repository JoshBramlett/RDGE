//! \headerfile <rdge/graphics/animation.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 02/15/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/spritesheet.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class Animation
//! \brief Storage and logic to represent an animated sequence
//! \details Stores a collection of \ref texture_part objects which represent
//!          the individual frames in the animation.  Elapsed ticks are
//!          accumulated in a local cache and the resultant frame will be
//!          provided based upon that and the specified \ref PlayMode.
class Animation
{
public:
    //! \enum PlayMode
    //! \brief Defines how the animation will play
    enum class PlayMode : uint8
    {
        Normal,      //!< Single iteration
        Reverse,     //!< Single iteration starting with the end frame
        Loop,        //!< Loop starting with the start frame
        LoopReverse, //!< Loop starting with the end frame
        LoopPingPong //!< Loop going from front to back to front
    };

    //! \brief Animation ctor
    //! \param [in] mode \ref PlayMode
    //! \param [in] interval Ticks between frame transitions
    explicit Animation (PlayMode mode = PlayMode::Normal, uint32 interval = 0);

    //! \brief Animation ctor
    //! \param [in] interval Ticks between frame transitions
    explicit Animation (uint32 interval);

    //! \brief Get the index of the current frame
    //! \param [in] ticks Elapsed ticks since last call
    //! \returns Index of the current frame
    uint32 GetFrameIndex (uint32 ticks) noexcept;

    //! \brief Get the \ref texture_part of the current frame
    //! \param [in] ticks Elapsed ticks since last call
    //! \returns Current frame
    const texture_part& GetFrame (uint32 ticks) noexcept;

    //! \brief Reset the animation sequence
    void Reset (void) noexcept;

    //! \brief Total duration (in ticks) of the animation
    //! \returns Animation length
    uint32 Duration (void) const noexcept;

    //! \brief Check if animation has finished
    //! \returns True iff the animation has completed, false otherwise
    bool IsFinished (void) const noexcept;

public:
    PlayMode mode = PlayMode::Normal; //!< How the animation is set to play

    uint32 interval = 0; //!< Interval between frames
    uint32 elapsed = 0;  //!< Elapsed time since the start of the animation

    std::vector<texture_part> frames; //!< Collection of the frames
};

} // namespace rdge
