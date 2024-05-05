#pragma once

#include "event/ApplicationEvent.h"
#include "IWindow.h"
#include "LayerStack.h"
#include "core/Logging.h"
#include "core/Asserts.h"

namespace engine
{
    class Application
    {
    public:
        Application();

        virtual ~Application();

        void Run();

        void OnEvent(engine::Event& e);

        void PushLayer(Layer* layer);

        void PushOverlay(Layer* layer);

        inline IWindow& GetWindow() { return *m_Window; }

        static Application& Get() { return *s_Instance; }

    private:
        bool OnWindowClose(WindowCloseEvent& e);

    private:
        std::unique_ptr<IWindow> m_Window;
        bool m_Running = true;
        LayerStack m_LayerStack;
        static Application* s_Instance;

    };
//should be implemented by the client
    Application* CreateApplication();

} // namespace BZEngine