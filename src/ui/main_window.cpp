#include "../common.h"
#include "../opengl_shared.h"
#include "../imgui_shared.h"
#include "../application.h"

#include "main_window.h"
#include "main_window_widgets.h"

char g_IMGuiIniPath[1024];

MainWindow::MainWindow(const char* title)
{
    m_vSize = {1366, 768};

    SetupSDL(title);
    InitImGUI();
}

void MainWindow::SetupSDL(const char* title)
{
    m_pSDLWindow = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_vSize[0], m_vSize[1],
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

    m_pWidgets = new MainWindowWidgets(this);

}

MainWindow::~MainWindow()
{
    if (m_pGLContext)
        SDL_GL_DeleteContext(m_pGLContext);

    if (m_pSDLWindow)    
        SDL_DestroyWindow(m_pSDLWindow);

    if (m_pWidgets)
        delete m_pWidgets;
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

        m_vSize[0] = width;
        m_vSize[1] = height;
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
        // HandleKeyDown(event);
        return true;

    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEWHEEL:
    case SDL_KEYUP:
        // PropagateControlsEvent(event);
        return true;

    case SDL_DROPFILE:
        // HandleDropfileEvent(event);
        return true;
    }

    return true;
}

int MainWindow::Update()
{
    glViewport(0, 0, m_vSize[0], m_vSize[1]);
    glClear( GL_COLOR_BUFFER_BIT );

    ImGui::SetCurrentContext(m_pImGuiContext);


    SDL_Event event;
    while( SDL_PollEvent( &event ) )
    {
        if (!CommonHandleEvent(event))
            return 1;
    }

    ImGuiBeginFrame();
    
    m_pWidgets->Render();

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

    // ImGuiHelpers::Init();

    // int32_t cursorData[2] = {0, 0};
    // g_EmptyCursor         = SDL_CreateCursor((Uint8 *)cursorData, (Uint8 *)cursorData, 8, 8, 4, 4);
    // SDL_SetCursor(g_EmptyCursor);

    // Force backend to initialize
    //GLBackend::Instance();

    
    // InitBackgroundRenderer();
    
}