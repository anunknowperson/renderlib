#pragma once

#include <cstdint>
#include <functional>
#include "event/Event.h"

namespace engine
{
    struct WindowProps
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        WindowProps(const std::string& title = "Renderlib",
                    uint32_t width = 1280,
                    uint32_t height = 720)
                : Title(title), Width(width), Height(height)
        {
        }
    };
    class IWindow
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~IWindow() = default;

        virtual void OnUpdate() = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        // Window attributes
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void* GetNativeWindow() const = 0;

        static IWindow* Create(const WindowProps& props = WindowProps());
    };
} // namespace engine