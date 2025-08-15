#include "application.h"
#include "opengl_shared.h"
#include "main_window.h"

#include "variant.h"

#include "imgui_helpers.h"

#include "ui_styles_manager.h"
#include "ui_fancy_background_renderer.h"

#include "main_window_widgets.h"

#include "r_camera_controller.h"
#include "r_scene_renderer.h"
#include "commands_registry.h"

#include "imgui_internal.h"

char g_IMGuiIniPath[1024];

MainWindow::MainWindow(const char* title)
{
    m_iWindowWidth = 1366;
    m_iWindowHeight = 768;

    SetupSDL(title);
    InitImGUI();

    m_pWidgets = new MainWindowWidgets(this);
    m_pFancyBackgroundRenderer = new FancyBackgroundRenderer();

    m_pSceneRenderer = new SceneRenderer();
    m_pCameraController = new CameraController(&m_Viewport);
}

MainWindow::~MainWindow()
{
    if (m_pGLContext)
        SDL_GL_DeleteContext(m_pGLContext);

    if (m_pSDLWindow)    
        SDL_DestroyWindow(m_pSDLWindow);

    
    delete m_pWidgets;
    delete m_pFancyBackgroundRenderer;
    delete m_pSceneRenderer;
    delete m_pCameraController;
    
}

void MainWindow::SetupSDL(const char* title)
{
    m_pSDLWindow = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_iWindowWidth, m_iWindowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    
    if (m_pSDLWindow == nullptr) {
        throw std::string("SDL_CreateWindow() failed");
    }

    
    m_pGLContext = SDL_GL_CreateContext(m_pSDLWindow);
    if (m_pGLContext == NULL) {
        throw std::string("SDL_GL_CreateContext() failed");
    }

    printf("OpenGL loaded\n");
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));

    

}



bool MainWindow::CheckImGuiEvent(SDL_Event &event)
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        if (ImGui::GetIO().WantCaptureKeyboard)
            return true;
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        break;
    }

    return false;
}

bool MainWindow::HandleWindowStateEvent(SDL_Event &e)
{
    bool bUpdateViewport = false;

    switch (e.window.event)
    {
    case SDL_WINDOWEVENT_CLOSE:
        Application::Instance()->Terminate();
        return false;
    case SDL_WINDOWEVENT_RESTORED:
        m_windowState   = WindowState::Restored;
        bUpdateViewport = true;
        break;
    case SDL_WINDOWEVENT_MAXIMIZED:
        m_windowState   = WindowState::Maxmized;
        bUpdateViewport = true;
        break;
    case SDL_WINDOWEVENT_MINIMIZED:
        m_windowState   = WindowState::Minimized;
        bUpdateViewport = true;
        break;
    case SDL_WINDOWEVENT_ENTER:
        m_bHasMouse = true;
        break;
    case SDL_WINDOWEVENT_LEAVE:
        m_bHasMouse = false;
        break;
    case SDL_WINDOWEVENT_RESIZED:
    case SDL_WINDOWEVENT_SIZE_CHANGED:
        bUpdateViewport = true;
        break;
    }

    if (bUpdateViewport)
    {
        int width, height;

        SDL_GetWindowSize(m_pSDLWindow, &width, &height);
        glViewport(0, 0, height, height);

        m_iWindowWidth = width;
        m_iWindowHeight = height;
    }

    return true;
}



bool MainWindow::CommonHandleEvent(SDL_Event &event)
{
    ImGui::SetCurrentContext(m_pImGuiContext);
    ImGui_ImplSDL2_ProcessEvent(&event);

    if (CheckImGuiEvent(event))
        return true;

    switch (event.type)
    {
    case SDL_QUIT:
        return false;

    case SDL_WINDOWEVENT:
        return HandleWindowStateEvent(event);

    case SDL_KEYDOWN:
        HandleKeyDown(event);
        return true;

    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEWHEEL:
    case SDL_KEYUP:
        PropagateControlsEvent(event);
        return true;

    case SDL_DROPFILE:
        // HandleDropfileEvent(event);
        return true;
    }

    return true;
}

int MainWindow::Update()
{
    UpdateTimers();    
    GL_BeginFrame();

    ImGui::SetCurrentContext(m_pImGuiContext);


    SDL_Event event;
    while( SDL_PollEvent( &event ) )
    {
        if (!CommonHandleEvent(event))
            return 1;
    }

    ImGuiBeginFrame();
    
    float toolbarHeight = 0;

    m_pWidgets->RenderMainMenu();
    gIDMainDockspace = DockSpaceOverViewport(toolbarHeight, ImGuiDockNodeFlags_PassthruCentralNode, 0);

    ImGui::ShowDemoWindow();

    if (ImGui::Begin("Debug"))
    {
        auto pos = m_pCameraController->GetPosition();
        auto ang = m_pCameraController->GetAngles();
        ImGui::Text("Camera pos: %f %f %f", pos.x, pos.y, pos.z);
        ImGui::Text("Camera ang: %f %f %f", ang.x, ang.y, ang.z);
        ImGui::End();
    }


    m_pWidgets->Render();


    m_pSceneRenderer->Render(m_pCameraController);

    ImGuiEndFrame();

    SDL_GL_SwapWindow( m_pSDLWindow );
    SDL_Delay( 1 );    

    return 1;
}

void MainWindow::ImGuiEndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MainWindow::ImGuiBeginFrame()
{
    if (m_bUpdateImGuiStyleNextFrame)
        UIStyles::Manager::Instance()->ApplyCurrentStyle();
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void MainWindow::InitImGUI()
{
    IMGUI_CHECKVERSION();
    m_pImGuiContext = ImGui::CreateContext();
    ImGuiIO &io     = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NoMouseCursorChange;

    char *p = SDL_GetPrefPath(SDL_ORGANIZATION, SDL_APP_NAME);
    snprintf(g_IMGuiIniPath, sizeof(g_IMGuiIniPath), "%s/imgui.ini", p);

    io.IniFilename = g_IMGuiIniPath;

    ImGuiStyle *style = &ImGui::GetStyle();

    style->FrameBorderSize = 1;
    style->TabBorderSize   = 1;
    style->WindowRounding  = 4;

    // setup platform/renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(m_pSDLWindow, m_pGLContext);

    ImGui_ImplOpenGL3_Init("#version 330");

    UIStyles::Manager::Instance()->ApplyCurrentStyle();

    ImGuiHelpers::Init();

    // int32_t cursorData[2] = {0, 0};
    // g_EmptyCursor         = SDL_CreateCursor((Uint8 *)cursorData, (Uint8 *)cursorData, 8, 8, 4, 4);
    // SDL_SetCursor(g_EmptyCursor);

    // Force backend to initialize
    //GLBackend::Instance();

    
    // InitBackgroundRenderer();
    
}

void MainWindow::GL_BeginFrame()
{
    m_Viewport.ApplyGL();
    

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    glClearDepth(1);

    GL_CheckForErrors();

    glDisable(GL_MULTISAMPLE);

    GL_CheckForErrors();

    // glClearColor(0.25, .25, .25, 1);

    m_pFancyBackgroundRenderer->PerformRendering();
    GL_CheckForErrors();

    glClear(GL_DEPTH_BUFFER_BIT);

    // GLScreenSpace2DRenderer::Instance()->NewFrame(m_i3DViewport);
    GL_CheckForErrors();
}

ImGuiID MainWindow::DockSpaceOverViewport(float heightAdjust, ImGuiDockNodeFlags dockspace_flags,
                                               const ImGuiWindowClass *window_class)
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();

    ImVec2 pos  = ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + heightAdjust);
    ImVec2 size = ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - heightAdjust);

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags host_window_flags = 0;
    host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
    host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        host_window_flags |= ImGuiWindowFlags_NoBackground;

    char label[32];
    snprintf(label, IM_ARRAYSIZE(label), "DockSpaceViewport_%08X", viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0);

    ImGui::Begin(label, NULL, host_window_flags);

    ImGui::PopStyleVar(3);

    char label2[32];
    snprintf(label2, IM_ARRAYSIZE(label), "DockSpace_%d", 1);

    m_DockSpaceId = ImGui::GetID(label2);

    ImGui::DockSpace(m_DockSpaceId, ImVec2(0.0f, 0.0f), dockspace_flags, window_class);

    // ImGui::ShowStyleSelector("Select style");

    auto c = ImGui::DockBuilderGetCentralNode(m_DockSpaceId);

    
    Viewport oldViewport = m_Viewport;

    if (c)
    {
        m_Viewport.SetClientArea(
                    (int)c->Pos.x,
                    (int)m_iWindowHeight - (int)(c->Pos.y + c->Size.y),
                    (int)c->Size.x,
                    (int)c->Size.y);

        
    }
    else
    {
        m_Viewport.SetClientArea(0, 0, m_iWindowWidth, m_iWindowHeight);
    }

    for (int i = 0; i < 4; i++)
    {
        if (oldViewport != m_Viewport)
        {
            // ViewportsOrchestrator::Instance()->FlagRepaintAll();
            break;
        }
    }

    ImGui::End();

    return m_DockSpaceId;
}

void MainWindow::HandleKeyDown(SDL_Event &event)
{
    switch (event.key.keysym.sym)
    {
    case SDLK_ESCAPE:        
        return;
    }

    if (Application::CommandsRegistry()->OnKeyDown())
        return;

    if (PropagateControlsEvent(event))
        return;
}

bool MainWindow::PropagateControlsEvent(SDL_Event &event)
{
    bool bWasHandled = false;

    //for (auto &it : EventHandlers())
    //{
        IEventHandler *it = m_pCameraController;

        int result = m_pCameraController->HandleEvent(bWasHandled, event, m_TimersData.frame_delta / 1000);

        bWasHandled = bWasHandled || result & EVENT_HANDLED;

        //if (result & EVENT_CONSUMED)
            //break;
    //}

    return bWasHandled;
}

void MainWindow::InitTimers()
{
    m_TimersData.timestamp_now          = SDL_GetPerformanceCounter();
    m_TimersData.timestamp_last         = SDL_GetPerformanceCounter();
    m_TimersData.frames_until_init      = 3;
    m_TimersData.fps_accum              = 0;
    m_TimersData.num_frames_this_second = 0;
}

float MainWindow::FrameDelta()
{
    return m_TimersData.frame_delta / 1000;
}

void MainWindow::UpdateTimers()
{
    m_TimersData.timestamp_last = m_TimersData.timestamp_now;
    m_TimersData.timestamp_now  = SDL_GetPerformanceCounter();

    m_TimersData.frame_delta = (double)((m_TimersData.timestamp_now - m_TimersData.timestamp_last) * 1000 /
                                        (double)SDL_GetPerformanceFrequency());
    m_TimersData.fps_accum += m_TimersData.frame_delta;

    if (m_TimersData.fps_accum > 1000)
    {
        m_TimersData.actual_fps             = m_TimersData.num_frames_this_second;
        m_TimersData.num_frames_this_second = 0;
        m_TimersData.fps_accum              = 0;
    }

    m_TimersData.num_frames_this_second++;
}