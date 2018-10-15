#include <rdge/debug/widgets/memory_widget.hpp>
#include <rdge/debug/renderer.hpp>
#include <rdge/debug/memory.hpp>
#include <rdge/util/memory/alloc.hpp>

#include <imgui/imgui.h>
#include <SDL_stdinc.h>

namespace rdge {
namespace debug {

#ifdef RDGE_DEBUG_MEMORY_TRACKER
memory_bucket_data g_memoryBuckets[memory_bucket_count] =
{
    { rdge::to_string(memory_bucket_none), 0, 0, 0, 0 },
    { rdge::to_string(memory_bucket_stbi), 0, 0, 0, 0 },
    { rdge::to_string(memory_bucket_sdl), 0, 0, 0, 0 },
    { rdge::to_string(memory_bucket_debug), 0, 0, 0, 0 },
    { rdge::to_string(memory_bucket_assets), 0, 0, 0, 0 },
    { rdge::to_string(memory_bucket_graphics), 0, 0, 0, 0 },
    { rdge::to_string(memory_bucket_physics), 0, 0, 0, 0 },
    { rdge::to_string(memory_bucket_allocators), 0, 0, 0, 0 },
    { rdge::to_string(memory_bucket_containers), 0, 0, 0, 0 }
};
#endif

void
MemoryWidget::UpdateWidget (void)
{
    using namespace rdge::debug::settings::memory;

    if (!show_widget)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(550.f, 480.f), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Memory Tracker", &show_widget))
    {
        ImGui::End();
        return;
    }

#ifndef RDGE_DEBUG_MEMORY_TRACKER
    ImGui::Spacing();
    ImGui::Text("Memory tracking disabled");
    ImGui::Spacing();

    ImGui::End();
    return;
#else
    if (ImGui::CollapsingHeader("Tracked Activity"))
    {
        ImGui::Columns(5, "tracked_memory");
        ImGui::Separator();
        ImGui::Text("name"); ImGui::NextColumn();
        ImGui::Text("resident"); ImGui::NextColumn();
        ImGui::Text("allocs"); ImGui::NextColumn();
        ImGui::Text("frees"); ImGui::NextColumn();
        ImGui::Text("reallocs"); ImGui::NextColumn();
        ImGui::Separator();

        uint64 total_resident = 0;
        size_t total_allocs = 0;
        size_t total_frees = 0;
        size_t total_reallocs = 0;
        for (size_t i = 0; i < static_cast<size_t>(memory_bucket_count); i++)
        {
            auto& bucket = g_memoryBuckets[i];
            ImGui::Text("%s", bucket.name.c_str()); ImGui::NextColumn();
            ImGui::Text("%llu", bucket.resident); ImGui::NextColumn();
            ImGui::Text("%zu", bucket.allocs); ImGui::NextColumn();
            ImGui::Text("%zu", bucket.frees); ImGui::NextColumn();
            ImGui::Text("%zu", bucket.reallocs); ImGui::NextColumn();

            total_resident += bucket.resident;
            total_allocs += bucket.allocs;
            total_frees += bucket.frees;
            total_reallocs += bucket.reallocs;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("total"); ImGui::NextColumn();
        ImGui::Text("%llu", total_resident); ImGui::NextColumn();
        ImGui::Text("%zu", total_allocs); ImGui::NextColumn();
        ImGui::Text("%zu", total_frees); ImGui::NextColumn();
        ImGui::Text("%zu", total_reallocs); ImGui::NextColumn();
        ImGui::Columns(1);
    }

    ImGui::Separator();
    ImGui::Text("SDL_GetNumAllocations: %d", SDL_GetNumAllocations());
    ImGui::Separator();

    ImGui::End();
#endif
}

void
MemoryWidget::OnWidgetCustomRender (void)
{ }

} // namespace debug
} // namespace rdge
