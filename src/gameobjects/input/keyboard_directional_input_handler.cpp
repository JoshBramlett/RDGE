#include <rdge/gameobjects/input/keyboard_directional_input_handler.hpp>
#include <rdge/math/functions.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <sstream>

namespace rdge {

KeyboardDirectionalInputHandler::KeyboardDirectionalInputHandler (Direction facing)
    : m_facing(facing)
{ }

KeyboardDirectionalInputHandler::KeyboardDirectionalInputHandler (ScanCode  keymap_up,
                                                                  ScanCode  keymap_left,
                                                                  ScanCode  keymap_down,
                                                                  ScanCode  keymap_right,
                                                                  Direction facing)
    : m_facing(facing)
{
    m_mapping.up    = keymap_up;
    m_mapping.left  = keymap_left;
    m_mapping.down  = keymap_down;
    m_mapping.right = keymap_right;
}

ScanCode&
KeyboardDirectionalInputHandler::operator[] (Direction direction)
{
    // NOTE Alternative solution could be to store the keys in an array
    //      and use the least significant bit intrinsic to compute the
    //      index.  I don't believe there'd be any noticable performance
    //      gain for changing it, but it's worth noting.

    switch (direction)
    {
    case Direction::NORTH:
        return m_mapping.up;
    case Direction::EAST:
        return m_mapping.right;
    case Direction::SOUTH:
        return m_mapping.down;
    case Direction::WEST:
        return m_mapping.left;
    default:
        break;
    }

    std::ostringstream ss;
    ss << "KeyboardDirectionalInputHandler subscript input is out of range."
       << " value=" << static_cast<uint32>(direction);

    RDGE_THROW(ss.str());
}

void
KeyboardDirectionalInputHandler::OnEvent (const Event& event)
{
    // NOTE A persisted state is required to work-around the native behavior SDL
    //      provides when handling multiple key presses.  The behavior being SDL
    //      will only send events for the last button pressed, unless there is a
    //      state change.  Meaning, press and hold 'a', then press and hold 's'
    //      will result in only 's' events being sent until either are released
    //      or another key is pressed.
    //
    //      The alternative work-around would be to check which keys are pressed
    //      every frame, but the former solution was chosen because it results in
    //      less work in the critical path.  However, it comes that the risk that
    //      any events that are suppressed will corrupt the internal state, so
    //      it's imperative all keyboard events are processed.

    if (event.IsKeyboardEvent())
    {
        auto args = event.GetKeyboardEventArgs();
        if (args.IsRepeating())
        {
            return;
        }

        auto key = args.PhysicalKey();
        Direction key_dir = Direction::NONE;
        if (key == m_mapping.up) {
            key_dir = Direction::NORTH;
        } else if (key == m_mapping.left) {
            key_dir = Direction::WEST;
        } else if (key == m_mapping.down) {
            key_dir = Direction::SOUTH;
        } else if (key == m_mapping.right) {
            key_dir = Direction::EAST;
        }

        if (key_dir != Direction::NONE)
        {
            if (args.IsKeyPressed())
            {
                using ::operator==;
                if (m_stateMask == 0u)
                {
                    // whatever the first key press is - is the facing direction
                    m_facing = key_dir;
                }

                m_stateMask |= key_dir;
            }
            else
            {
                m_stateMask &= ~key_dir;
            }

            m_dirty = true;
        }
    }
}

std::pair<math::vec2, Direction>
KeyboardDirectionalInputHandler::Calculate (void)
{
    if (!m_dirty)
    {
        return std::make_pair(m_displacement, m_facing);
    }

    m_displacement = { 0.f, 0.f };

    // invalid key combinations
    static Direction ns = Direction::NORTH | Direction::SOUTH;
    static Direction ew = Direction::EAST | Direction::WEST;
    if (((m_stateMask & ns) == ns) || ((m_stateMask & ew) == ew))
    {
        return std::make_pair(m_displacement, m_facing);
    }

    // TODO Required b/c is_enum_bitmask operators are in the global scope.
    //      Investigate moving operators inside rdge namespace and see if they
    //      can still be used in the global namespace.  Also on line 87.
    using ::operator!=;
    m_displacement.y += ((m_stateMask & Direction::NORTH) != 0u) ? 1.f : 0.f;
    m_displacement.x += ((m_stateMask & Direction::EAST) != 0u) ? 1.f : 0.f;
    m_displacement.y += ((m_stateMask & Direction::SOUTH) != 0u) ? -1.f : 0.f;
    m_displacement.x += ((m_stateMask & Direction::WEST) != 0u) ? -1.f : 0.f;

    bool move_on_x = !math::fp_eq(m_displacement.x, 0.f);
    bool move_on_y = !math::fp_eq(m_displacement.y, 0.f);
    if (move_on_x && move_on_y)
    {
        m_displacement *= DIAGONAL_MAGNITUDE;
    }
    else if (move_on_x || move_on_y)
    {
        // set the facing direction if we're moving in a singular direction,
        // which solves the problem of finding the correct facing direction
        // when multiple keys are pressed - and subsequently released.
        m_facing = m_stateMask;
    }

    m_dirty = false;
    return std::make_pair(m_displacement, m_facing);
}

} // namespace rdge
