#include "core/ViewImpl.h"

#include <SDL2/SDL.h>
#include <SDL_events.h>

#include "graphics/vulkan/vk_engine.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"

ViewImpl::ViewImpl() {
    SDL_Init(SDL_INIT_VIDEO);
}

struct EventAdapter final : Event {
    explicit EventAdapter(const SDL_Event& event) : event(event) {}
    SDL_Event event;
};

void ViewImpl::render() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if (ImGui::Begin("background")) {
        VulkanEngine &engine = VulkanEngine::Get();
        ImGui::SliderFloat("Render Scale", &engine.renderScale, 0.3f, 1.f);
        // other code
    }
    ImGui::End();

    // make imgui calculate internal draw structures
    ImGui::Render();
}
