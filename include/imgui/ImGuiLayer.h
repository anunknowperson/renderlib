#pragma once

#include "imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_vulkan.h"

#include "SDL.h"
#include "core/Application.h"
#include "event/Event.h"
#include "core/Layer.h"


namespace engine
{
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event& e) override;

        void Begin();
        void End();

    private:
        float m_Time = 0.0f;
    };
}