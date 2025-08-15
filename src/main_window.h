/*
    ImPFXEditor, (c) CrazyRussian, 2025
*/

#pragma once


#include <SDL2/SDL.h>
#include "imgui_shared.h"

#include "viewport.h"

enum class WindowState
{
    Minimized = 0,
    Maxmized,
    Restored
};

struct timersData
{
    double frame_delta            = 0;
    int    actual_fps             = 0;
    uint64_t timestamp_now        = 0;
    uint64_t timestamp_last       = 0;
    int    frames_until_init      = 3;
    double fps_accum              = 0;
    int    num_frames_this_second = 0;
};


class FancyBackgroundRenderer;
class MainWindowWidgets;
class SceneRenderer;
class CameraController;

class MainWindow
{
    SDL_Window * m_pSDLWindow = 0;
    SDL_GLContext m_pGLContext;

    ImGuiContext* m_pImGuiContext = 0;

    void SetupSDL(const char* title);
    void InitImGUI();
    void InitTimers();

    // State
    
    int m_iWindowWidth;
    int m_iWindowHeight;

    bool m_bHasMouse;
    WindowState m_windowState;
    timersData m_TimersData;
    
    float FrameDelta();
    void  UpdateTimers();


    void ImGuiEndFrame();
    void ImGuiBeginFrame();

    // Events handling
    bool CommonHandleEvent(SDL_Event &event);
    bool CheckImGuiEvent(SDL_Event &event);
    bool HandleWindowStateEvent(SDL_Event &e);

    // Contents
    MainWindowWidgets* m_pWidgets = nullptr;
    FancyBackgroundRenderer* m_pFancyBackgroundRenderer = nullptr;
    SceneRenderer * m_pSceneRenderer = nullptr;

    Viewport m_Viewport;
    CameraController * m_pCameraController;

    bool m_bUpdateImGuiStyleNextFrame = false;

    void GL_BeginFrame();

    void HandleKeyDown(SDL_Event &event);
    bool PropagateControlsEvent(SDL_Event &event);


    // Docking
    ImGuiID                    gIDMainDockspace = 0;
    ImGuiID                    m_DockSpaceId = 0;
    ImGuiID DockSpaceOverViewport(float heightAdjust, ImGuiDockNodeFlags dockspace_flags,
                                            const ImGuiWindowClass *window_class);
public:
    MainWindow(const char* strTitle);
    ~MainWindow();

    int Update();

    SDL_Window * Handle() const
    {
        return m_pSDLWindow;
    }

    void FlagUpdateStyles() { m_bUpdateImGuiStyleNextFrame = true; }

    const int Width() const { return m_iWindowWidth; }
    const int Height() const { return m_iWindowHeight; }

    
};