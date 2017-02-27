#include <rdge/graphics/animation.hpp>

#include <SDL_assert.h>

#include <algorithm> // std::min, std::max

namespace rdge {

Animation::Animation (PlayMode mode, uint32 interval)
    : mode(mode)
    , interval(interval)
{ }

Animation::Animation (uint32 interval)
    : interval(interval)
{ }

uint32
Animation::GetFrameIndex (uint32 ticks) noexcept
{
    SDL_assert(this->interval != 0);
    SDL_assert(this->frames.size() != 0);

    size_t frame_count = this->frames.size();
    if (frame_count == 1)
    {
        return 0;
    }

    this->elapsed += ticks;
    uint32 frame = this->elapsed / this->interval;
    switch (this->mode)
    {
    case PlayMode::Normal:
        frame = std::min(frame, static_cast<uint32>(frame_count - 1));
        break;
    case PlayMode::Reverse:
        frame = std::max(((frame_count - 1) - frame), 0ul);
        break;
    case PlayMode::Loop:
        frame = frame % frame_count;
        break;
    case PlayMode::LoopReverse:
        frame = (frame_count - 1) - (frame % frame_count);
        break;
    case PlayMode::LoopPingPong:
        frame = frame % ((frame_count * 2) - 2);
        if (frame >= frame_count)
        {
            frame = (frame_count - 2) - (frame - frame_count);
        }
        break;
    default:
        frame = 0;
        break;
    }

    return frame;
}

const texture_part&
Animation::GetFrame (uint32 ticks) noexcept
{
    return this->frames[GetFrameIndex(ticks)];
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

} // namespace rdge
