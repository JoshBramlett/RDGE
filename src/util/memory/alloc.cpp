#include <rdge/util/memory/alloc.hpp>
#include <rdge/internal/hints.hpp>
#include <rdge/internal/hints.hpp>
#include <rdge/util/logger.hpp>

#include <SDL_assert.h>

#ifdef RDGE_DEBUG
#include <imgui/imgui.h>
#endif

#include <cstdlib>
#include <errno.h>

// Checks for arithmetic overflow on the (size * num) calculation.
// Returns 1 if an overflow will occur.
// see https://github.com/gagern/gnulib/blob/master/lib/safe-alloc.c
#define SAFE_ALLOC_OVERSIZED(n, s) \
  ((size_t) (sizeof (ptrdiff_t) <= sizeof (size_t) ? -1 : -2) / (s) < (n))

namespace rdge {
namespace detail {

namespace {

#ifdef RDGE_DEBUG_MEMORY_TRACKER
intrusive_list<memory_profile> s_profiles[memory_profile_subsystem_count];
memory_profile s_anonymousProfile;
#endif

} // anonymous namespace

bool
safe_alloc (void** p, size_t size, size_t num, bool clear, memory_profile* profile)
{
    if (size == 0 || num == 0)
    {
        SDL_assert(false); // Valid case, remove assert if intentional
        *p = nullptr;
        return true;
    }

    if (UNLIKELY(SAFE_ALLOC_OVERSIZED(num, size)))
    {
        SDL_assert(false);
        errno = ENOMEM;
        return false;
    }

#ifdef RDGE_DEBUG_MEMORY_TRACKER
    size_t total_size = (num * size) + sizeof(size_t);
    void* poffset = malloc(total_size);
    if (UNLIKELY(poffset == nullptr))
    {
        return false;
    }

    if (clear)
    {
        memset(poffset, 0, total_size);
    }

    memcpy(poffset, &total_size, sizeof(size_t));
    *p = ((uint8*)poffset) + sizeof(size_t);

    if (!profile)
    {
        profile = &s_anonymousProfile;
        DLOG() << "MALLOC size=" << total_size;
    }
    else
    {
        DLOG() << "MALLOC memory_profile[" << (void*)profile << "]"
               << " size=" << total_size;
    }

    profile->resident += total_size;
    profile->allocs++;
#else
    rdge::Unused(profile);

    *p = (clear) ? calloc(num, size) : malloc(num * size);
    if (UNLIKELY(*p == nullptr))
    {
        return false;
    }
#endif

    return true;
}

bool
safe_realloc (void** p, size_t size, size_t num, memory_profile* profile)
{
    if (size == 0 || num == 0)
    {
        SDL_assert(false); // Valid case, remove assert if intentional
        free(*p);
        *p = nullptr;
        return true;
    }

    if (UNLIKELY(SAFE_ALLOC_OVERSIZED(num, size)))
    {
        SDL_assert(false);
        errno = ENOMEM;
        return false;
    }

#ifdef RDGE_DEBUG_MEMORY_TRACKER
    uint8* actual_p = ((uint8*)*p) - sizeof(size_t);
    size_t old_size = *(size_t*)actual_p;

    size_t new_size = (num * size) + sizeof(size_t);
    void* poffset = realloc(actual_p, new_size);
    if (UNLIKELY(poffset == nullptr))
    {
        return false;
    }

    memcpy(poffset, &new_size, sizeof(size_t));
    *p = ((uint8*)poffset) + sizeof(size_t);

    if (!profile)
    {
        profile = &s_anonymousProfile;
        DLOG() << "REALLOC size=" << new_size;
    }
    else
    {
        DLOG() << "REALLOC memory_profile[" << (void*)profile << "]"
               << " size=" << new_size;
    }

    profile->resident += new_size - old_size;
    profile->reallocs++;
#else
    rdge::Unused(profile);

    void* tmp = realloc(*p, num * size);
    if (UNLIKELY(tmp == nullptr))
    {
        return false;
    }
    *p = tmp;
#endif

    return true;
}

#ifdef RDGE_DEBUG_MEMORY_TRACKER
void
debug_free (void** p, memory_profile* profile)
{
    if (*p == nullptr)
    {
        return;
    }

    uint8* actual_p = ((uint8*)*p) - sizeof(size_t);
    size_t size = *(size_t*)actual_p;

    if (!profile)
    {
        profile = &s_anonymousProfile;
    }

    profile->resident -= size;
    profile->frees++;

    free(actual_p);
    *p = nullptr;
}

void
register_memory_profile (memory_profile& profile, const char* name)
{
    profile.name = name;
    s_profiles[memory_profile_subsystem_none].push_back(&profile);
}

void
unregister_memory_profile (memory_profile& profile)
{
    s_profiles[memory_profile_subsystem_none].remove(&profile);
}
#endif

} // namespace detail

#ifdef RDGE_DEBUG
namespace debug {

void
ShowMemoryTracker (bool* p_open)
{
#ifdef RDGE_DEBUG_MEMORY_TRACKER
    using namespace rdge::detail;

    ImGui::SetNextWindowSize(ImVec2(550,680), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Memory Tracker", p_open))
    {
        ImGui::End();
        return;
    }

    if (ImGui::CollapsingHeader("Anonymous"))
    {
        ImGui::Columns(4, "anonymous_memory");
        ImGui::Separator();
        ImGui::Text("resident"); ImGui::NextColumn();
        ImGui::Text("allocs"); ImGui::NextColumn();
        ImGui::Text("frees"); ImGui::NextColumn();
        ImGui::Text("reallocs"); ImGui::NextColumn();
        ImGui::Separator();

        ImGui::Text("%llu", s_anonymousProfile.resident); ImGui::NextColumn();
        ImGui::Text("%zu", s_anonymousProfile.allocs); ImGui::NextColumn();
        ImGui::Text("%zu", s_anonymousProfile.frees); ImGui::NextColumn();
        ImGui::Text("%zu", s_anonymousProfile.reallocs); ImGui::NextColumn();
        ImGui::Columns(1);
    }

    memory_profile_subsystem ss = memory_profile_subsystem_none;
    size_t size = s_profiles[ss].size();
    if (size > 0 && ImGui::CollapsingHeader("Tracked"))
    {
        ImGui::Columns(5, "tracked_memory");
        ImGui::Separator();
        ImGui::Text("name"); ImGui::NextColumn();
        ImGui::Text("resident"); ImGui::NextColumn();
        ImGui::Text("allocs"); ImGui::NextColumn();
        ImGui::Text("frees"); ImGui::NextColumn();
        ImGui::Text("reallocs"); ImGui::NextColumn();
        ImGui::Separator();

        s_profiles[ss].for_each([](auto* p) {
            ImGui::Text("%s", p->name); ImGui::NextColumn();
            ImGui::Text("%llu", p->resident); ImGui::NextColumn();
            ImGui::Text("%zu", p->allocs); ImGui::NextColumn();
            ImGui::Text("%zu", p->frees); ImGui::NextColumn();
            ImGui::Text("%zu", p->reallocs); ImGui::NextColumn();
        });
        ImGui::Columns(1);
    }

    ss = memory_profile_subsystem_graphics;
    size = s_profiles[ss].size();
    if (size > 0 && ImGui::CollapsingHeader("Graphics"))
    {
        ImGui::Text("Nothing yet.");
    }

    ImGui::End();
#else
    rdge::Unused(p_open);
#endif
}

} // namespace debug
#endif

} // namespace rdge
