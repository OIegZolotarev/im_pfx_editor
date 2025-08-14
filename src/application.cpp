
#include <SDL2/SDL.h>
#include <opengl_shared.h>

#include "common.h"
#include "application.h"

#include "fs/core.h"
#include "persistent.h"

Application::Application(int argc, char * argv[])
{
    m_pMainWindow = new MainWindow("ImGUI PFX Editor");
    m_pFileSystem = new FileSystem();
    m_pPersistentStorage = new PersistentStorage(this);
}

Application::~Application()
{
    
    delete m_pPersistentStorage;
    delete m_pFileSystem;
    delete m_pMainWindow;

    printf("Application::~Application() stub\n");
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
        window = Instance()->m_pMainWindow->SDLHandle();

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "EPIC FAIL", tmp, window);
    exit(1);
}

PersistentStorage * Application::GetPersistentStorage()
{

}