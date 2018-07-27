//! \headerfile <rdge/gameobjects/input/keyboard_directional_input_handler.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 03/17/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/events/event.hpp>
#include <rdge/gameobjects/input/ihandler.hpp>
#include <rdge/gameobjects/types.hpp>
#include <rdge/math/vec2.hpp>
#include <rdge/system/keyboard.hpp>

#include <utility>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

//! \class KeyboardDirectionalInputHandler
//! \brief Persisted state handing of keyboard directional (e.g. WASD) input
//! \details Allows configurable mapping of NESW keys, and handles keyboard events
//!          to create a stateful representation of the directional movement per
//!          frame.  Calculate should be called after all events have been processed
//!          for a given frame, and the resulting vector represents the x and y
//!          coordinate around a unit circle.
//! \warning All keyboard events must be passed through.  If any applicable events
//!          are missed the internal state will become corrupted and reported
//!          values will not be correct.
class KeyboardDirectionalInputHandler : public IDirectionalInputHandler
{
public:

    //! \brief Default KeyboardDirectionalInputHandler ctor
    //! \details Key mappings default to WASD.
    //! \param [in] facing Initial facing direction
    KeyboardDirectionalInputHandler (Direction facing = Direction::SOUTH);

    //! \brief KeyboardDirectionalInputHandler ctor
    //! \details Provides setup for custom key bindings.
    //! \param [in] keymap_up Key bound to the up (north) direction
    //! \param [in] keymap_left Key bound to the left (west) direction
    //! \param [in] keymap_down Key bound to the down (south) direction
    //! \param [in] keymap_right Key bound to the right (east) direction
    //! \param [in] facing Initial facing direction
    explicit KeyboardDirectionalInputHandler (ScanCode  keymap_up,
                                              ScanCode  keymap_left,
                                              ScanCode  keymap_down,
                                              ScanCode  keymap_right,
                                              Direction facing = Direction::SOUTH);

    //! \brief KeyboardDirectionalInputHandler subscript operator
    //! \details Retrieves key mapping by \ref Direction.  Key mappings exist for
    //!          only cardinal (NESW) directions, and the returned result is a
    //!          reference to allow setting a new mapping.  e.g.
    //! \code{.cpp}
    //! my_handler[Direction::NORTH] = ScanCode::UP;
    //! \endcode
    //! \param [in] direction Direction mapping
    //! \returns Reference to the mapped key
    //! \throws rdge::Exception Out of range lookup
    ScanCode& operator[] (Direction direction);

    //! \brief Event handler
    void OnEvent (const Event& event) override;

    //! \brief Calculate the relative displacement for a frame
    //! \details See \ref IDirectionalInputHandler for details.
    //! \returns Pair containing the relative displacement and a NESW direction
    std::pair<math::vec2, Direction> Calculate (void) override;

    //! \brief Get the current facing direction
    Direction GetDirection (void) const noexcept { return m_facing; }

    //! \brief Reset initial facing direction
    //! \details Performs a reset on the state machine, so it's use should be
    //!          limited to initialization routines.
    //! \param [in] facing Facing direction
    void ResetDirection (Direction facing);

private:

    //! \brief Unit circle coordinates for a 45 degree directional vector
    //! \note Value computed from [sin(45deg), cos(45deg)], or more simply
    //!       sqrt(1/2) by solving for equal opposite/adjacent edges in a
    //!       right triangle (where the hypotenuse is 1).
    static constexpr float DIAGONAL_MAGNITUDE = 0.707106781187f;

    math::vec2 m_displacement;                //!< Displacement unit vector
    Direction  m_stateMask = Direction::NONE; //!< Bitmask of key press states
    Direction  m_facing = Direction::NONE;    //!< Facing direction (limited to NESW)

    bool m_dirty = false; //!< Denotes a state change between displacement calculations

    //! \struct button_mapping
    //! \brief Denotes which physical keys are mapped to movement directions
    struct button_mapping
    {
        ScanCode up    = ScanCode::W; //!< Up (north)
        ScanCode left  = ScanCode::A; //!< Left (west)
        ScanCode down  = ScanCode::S; //!< Down (south)
        ScanCode right = ScanCode::D; //!< Right (east)
    } m_mapping;
};

} // namespace rdge
