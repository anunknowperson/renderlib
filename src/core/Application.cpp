#include "core/Application.h"

namespace engine
{

    Application* Application::s_Instance = nullptr;

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
    Application::Application()
    {
#ifndef DEBUG
        if (s_Instance) LOGE("Application already exists!");
#endif
        s_Instance = this;

        m_Window = std::unique_ptr<IWindow>(IWindow::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
    }

    Application::~Application()
    {

    }

    void Application::OnEvent(engine::Event &e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

        //BZ_CORE_INFO("{0}", e.ToString());

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
                break;
        }
    }

    void Application::Run()
    {
        while (m_Running)
        {
            for (Layer* layer : m_LayerStack)
            {
                layer->OnUpdate();
            }
            m_Window->OnUpdate();

        }
    }

// TODO: add layers attach
    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

} // namespace engine