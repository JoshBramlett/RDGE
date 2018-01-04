//! \headerfile <rdge/graphics/animation.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 02/15/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/spritesheet_region.hpp>

#include <vector>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class Animation
//! \brief Storage and logic to represent an animated sequence
//! \details Stores a collection of \ref spritesheet_region objects that are
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
        NORMAL,      //!< Single iteration
        REVERSE,     //!< Single iteration starting with the end frame
        LOOP,        //!< Loop starting with the start frame
        LOOPREVERSE, //!< Loop starting with the end frame
        LOOPPINGPONG //!< Loop going from front to back to front
    };

    //! \brief Animation ctor
    //! \param [in] interval Ticks between frame transitions
    //! \param [in] mode \ref PlayMode
    explicit Animation (uint32 interval, PlayMode mode = PlayMode::NORMAL);

    //!@{ Animation default ctor/dtor
    Animation (void) = default;
    ~Animation (void) noexcept = default;
    //!@}

    //! \brief Get the index of the current frame
    //! \param [in] ticks Elapsed ticks since last call
    //! \returns Index of the current frame
    uint32 GetFrameIndex (uint32 ticks) noexcept;

    //! \brief Get the \ref spritesheet_region of the current frame
    //! \param [in] ticks Elapsed ticks since last call
    //! \returns Current frame
    const spritesheet_region& GetFrame (uint32 ticks);

    //! \brief Reset the animation sequence
    void Reset (void) noexcept;

    //! \brief Total duration (in ticks) of the animation
    //! \returns Animation length
    uint32 Duration (void) const noexcept;

    //! \brief Check if animation has finished
    //! \returns True iff the animation has completed, false otherwise
    bool IsFinished (void) const noexcept;

public:
    PlayMode mode = PlayMode::NORMAL; //!< How the animation is set to play

    uint32 interval = 0; //!< Interval between frames
    uint32 elapsed = 0;  //!< Elapsed time since the start of the animation

    std::vector<spritesheet_region> frames; //!< Collection of the frames
};

//! \brief Animation::PlayMode stream output operator
std::ostream& operator<< (std::ostream&, Animation::PlayMode);

//!@{ Animation::PlayMode string conversions
std::string to_string (Animation::PlayMode);
bool try_parse (const std::string&, Animation::PlayMode&);
//!@}

} // namespace rdge
