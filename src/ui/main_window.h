/*
    ImPFXEditor, (c) CrazyRussian, 2025
*/

#pragma once


#include <SDL2/SDL.h>
#include "../imgui_shared.h"

enum class WindowState
{
    Minimized = 0,
    Maxmized,
    Restored
};

class MainWindowWidgets;

class MainWindow
{
    SDL_Window * m_pSDLWindow = 0;
    SDL_GLContext m_pGLContext;

    ImGuiContext* m_pImGuiContext = 0;

    void SetupSDL(const char* title);
    void InitImGUI();

    // State
    glm::vec2 m_vSize;
    bool m_bHasMouse;
    WindowState m_windowState;

    void ImGuiEndFrame();
    void ImGuiBeginFrame();

    // Events handling
    bool CommonHandleEvent(SDL_Event &event);
    bool CheckImGuiEvent(SDL_Event &event);
    bool HandleWindowStateEvent(SDL_Event &e);

    MainWindowWidgets* m_pWidgets = nullptr;

public:
    MainWindow(const char* strTitle);
    ~MainWindow();

    int Update();

    SDL_Window * SDLHandle()
    {
        return m_pSDLWindow;
    }

};