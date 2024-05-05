#include "core/SDLWindow.h"
#include "Asserts.h"
#include "core/Logging.h"
#include "event/ApplicationEvent.h"
#include "event/KeyEvent.h"
#include "event/MouseEvent.h"

namespace engine {

    static bool s_SDLInitialized = false;

    IWindow* IWindow::Create(const WindowProps& props)
    {
        return new SDLWindow(props);
    }

    SDLWindow::SDLWindow(const WindowProps& props)
    {
        Init(props);
    }

    SDLWindow::~SDLWindow()
    {
        Shutdown();
    }

    void SDLWindow::Init(const WindowProps& props)
    {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;

        LOGI("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        if (!s_SDLInitialized)
        {
            int success = SDL_Init(SDL_INIT_VIDEO);
#ifndef DEBUG
      if (success == 0) LOGE("Failed to initialize window!")
#endif
            s_SDLInitialized = true;
        }

        m_Window = SDL_CreateWindow(props.Title.c_str(),
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    props.Width,
                                    props.Height,
                                    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
#ifndef DEBUG
        if (m_Window == nullptr) LOGE("Failed to create window!")
#endif

        SDL_GL_MakeCurrent(m_Window, SDL_GL_CreateContext(m_Window));
        SDL_SetWindowData(m_Window, "m_Data", &m_Data);
        SetVSync(true);
    }

    void SDLWindow::Shutdown()
    {
        SDL_DestroyWindow(m_Window);
    }

    void SDLWindow::OnUpdate()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_WINDOWEVENT:
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_RESIZED:
                        {
                            int newWidth = event.window.data1;
                            int newHeight = event.window.data2;
                            m_Data.Width = newWidth;
                            m_Data.Height = newHeight;

                            WindowResizeEvent resizeEvent(newWidth, newHeight);
                            m_Data.EventCallback(resizeEvent);
                            break;
                        }
                        case SDL_WINDOWEVENT_CLOSE:
                        {
                            WindowCloseEvent closeEvent;
                            m_Data.EventCallback(closeEvent);
                            break;
                        }
                    }
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                {
                    int key = event.key.keysym.sym;
                    switch (event.key.state)
                    {
                        case SDL_PRESSED:
                        {
                            KeyPressedEvent keyPressedEvent(key, event.key.repeat);
                            m_Data.EventCallback(keyPressedEvent);
                            break;
                        }
                        case SDL_RELEASED:
                        {
                            KeyReleasedEvent keyReleasedEvent(key);
                            m_Data.EventCallback(keyReleasedEvent);
                            break;
                        }
                    }
                    break;
                }
                case SDL_TEXTINPUT:
                {
                    unsigned int keycode = event.text.text[0];
                    KeyTypedEvent keyTypedEvent(keycode);
                    m_Data.EventCallback(keyTypedEvent);
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                {
                    int button = event.button.button;
                    switch (event.button.state)
                    {
                        case SDL_PRESSED:
                        {
                            MouseButtonPressedEvent mouseButtonPressedEvent(button);
                            m_Data.EventCallback(mouseButtonPressedEvent);
                            break;
                        }
                        case SDL_RELEASED:
                        {
                            MouseButtonReleasedEvent mouseButtonReleasedEvent(button);
                            m_Data.EventCallback(mouseButtonReleasedEvent);
                            break;
                        }
                    }
                    break;
                }
                case SDL_MOUSEWHEEL:
                {
                    float xOffset = event.wheel.x;
                    float yOffset = event.wheel.y;
                    MouseScrolledEvent mouseScrolledEvent(xOffset, yOffset);
                    m_Data.EventCallback(mouseScrolledEvent);
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    float xPos = event.motion.x;
                    float yPos = event.motion.y;
                    MouseMovedEvent mouseMovedEvent(xPos, yPos);
                    m_Data.EventCallback(mouseMovedEvent);
                    break;
                }
                    // Handle other events...
            }
        }
        SDL_GL_SwapWindow(m_Window);
    }

    void SDLWindow::SetVSync(bool enabled)
    {
        m_Data.VSync = enabled;
        SDL_GL_SetSwapInterval(enabled ? 1 : 0);
    }

    bool SDLWindow::IsVSync() const
    {
        return m_Data.VSync;
    }
} // namespace RLEngine