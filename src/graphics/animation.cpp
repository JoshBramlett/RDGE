#include <rdge/graphics/animation.hpp>
#include <rdge/util/strings.hpp>

#include <SDL_assert.h>

#include <cstring> // strrchr
#include <algorithm> // std::min, std::max
#include <sstream>

namespace rdge {

Animation::Animation (uint32 interval, PlayMode mode)
    : mode(mode)
    , interval(interval)
{ }

uint32
Animation::GetFrameIndex (uint32 ticks) noexcept
{
    SDL_assert(this->mode == PlayMode::NONE || this->interval > 0);
    SDL_assert(this->frames.empty() == false);

    const size_t frame_count = this->frames.size();
    if (frame_count == 1)
    {
        return 0;
    }

    this->elapsed += ticks;
    uint32 frame = this->elapsed / this->interval;
    switch (this->mode)
    {
    case PlayMode::NORMAL:
        frame = std::min(frame, static_cast<uint32>(frame_count - 1));
        break;
    case PlayMode::REVERSE:
        frame = std::max(((frame_count - 1) - frame), 0ul);
        break;
    case PlayMode::LOOP:
        frame = frame % frame_count;
        break;
    case PlayMode::LOOPREVERSE:
        frame = (frame_count - 1) - (frame % frame_count);
        break;
    case PlayMode::LOOPPINGPONG:
        frame = frame % ((frame_count * 2) - 2);
        if (frame >= frame_count)
        {
            frame = (frame_count - 2) - (frame - frame_count);
        }
        break;
    case PlayMode::NONE:
    default:
        frame = 0;
        break;
    }

    return frame;
}

const animation_frame&
Animation::GetFrame (uint32 ticks)
{
    SDL_assert(this->frames.empty() == false);
    return this->frames.at(GetFrameIndex(ticks));
}

void
Animation::Reset (void) noexcept
{
    this->elapsed = 0;
}

uint32
Animation::Duration (void) const noexcept
{
    return this->frames.size() * this->interval;
}

bool
Animation::IsFinished (void) const noexcept
{
    return this->elapsed > Duration();
}

std::ostream&
operator<< (std::ostream& os, Animation::PlayMode value)
{
    return os << rdge::to_string(value);
}

std::string
to_string (Animation::PlayMode value)
{
    switch (value)
    {
#define CASE(X) case X: return (strrchr(#X, ':') + 1); break;
        CASE(Animation::PlayMode::NONE)
        CASE(Animation::PlayMode::NORMAL)
        CASE(Animation::PlayMode::REVERSE)
        CASE(Animation::PlayMode::LOOP)
        CASE(Animation::PlayMode::LOOPREVERSE)
        CASE(Animation::PlayMode::LOOPPINGPONG)
        default: break;
#undef CASE
    }

    std::ostringstream ss;
    ss << "UNKNOWN[" << static_cast<uint32>(value) << "]";
    return ss.str();
}

bool
try_parse (const std::string& test, Animation::PlayMode& out)
{
    std::string s = rdge::to_lower(test);
    if      (s == "none")         { out = Animation::PlayMode::NONE;         return true; }
    else if (s == "normal")       { out = Animation::PlayMode::NORMAL;       return true; }
    else if (s == "reverse")      { out = Animation::PlayMode::REVERSE;      return true; }
    else if (s == "loop")         { out = Animation::PlayMode::LOOP;         return true; }
    else if (s == "loopreverse")  { out = Animation::PlayMode::LOOPREVERSE;  return true; }
    else if (s == "looppingpong") { out = Animation::PlayMode::LOOPPINGPONG; return true; }

    return false;
}

} // namespace rdge
