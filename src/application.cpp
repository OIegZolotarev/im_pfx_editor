
#include <SDL2/SDL.h>
#include <opengl_shared.h>

#include "common.h"
#include "application.h"

// Application modules
#include "fs_core.h"
#include "persistent.h"
#include "commands_registry.h"


Application * gAppInstance = nullptr;

Application* Application::Instance()
{
    return gAppInstance;
}


Application::Application(int argc, char * argv[])
{
    gAppInstance = this;

    m_pFileSystem = new FileSystem();
    m_pPersistentStorage = new PersistentStorage(this);

    m_pCommandsRegistry = new CCommandsRegistry();    
    m_pCommandsRegistry->InitializeAllCommands();

    m_pMainWindow = new MainWindow("ImGUI PFX Editor");
    

}

Application::~Application()
{   
    delete m_pCommandsRegistry;

    delete m_pMainWindow; 
    delete m_pPersistentStorage;        
    delete m_pFileSystem;    
}

void Application::Run()
{
    glClearColor(1,0,1,1);

    while(!m_bTerminated)
        m_pMainWindow->Update();

}

FileSystem* Application::GetFileSystem()
{
    return Instance()->m_pFileSystem;
}

void Application::EPICFAIL(const char *format, ...)
{
    char tmp[4096];

    va_list argp;
    va_start(argp, format);
#ifdef WINDOWS
    vsprintf_s(tmp, sizeof(tmp), format, argp);
#else
    vsprintf(tmp, format, argp);
#endif
    va_end(argp);

    SDL_Window *window = nullptr;

    if (Instance()->m_pMainWindow)
        window = Instance()->m_pMainWindow->Handle();

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "EPIC FAIL", tmp, window);
    exit(1);
}

PersistentStorage * Application::GetPersistentStorage()
{
    return Instance()->m_pPersistentStorage;
}

CCommandsRegistry* Application::CommandsRegistry()
{
    return Instance()->m_pCommandsRegistry;
}

void Application::ShowMouseCursor()
{
    if (!m_bMouseCursorVisible)
        Con_Printf("Application::ShowMouseCursor()\n");
    m_bMouseCursorVisible = true;
}

void Application::HideMouseCursor()
{
    if (m_bMouseCursorVisible)
        Con_Printf("Application::HideMouseCursor()\n");

    m_bMouseCursorVisible = false;
}