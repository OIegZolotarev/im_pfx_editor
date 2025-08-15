/*
    ImPFXEditor, (c) CrazyRussian, 2025
*/

#pragma once

#include "main_window.h"

class FileSystem;
class PersistentStorage;
class CCommandsRegistry;

class Application
{      
    MainWindow * m_pMainWindow = nullptr;
    bool m_bTerminated = false;

    FileSystem * m_pFileSystem = nullptr;
    PersistentStorage* m_pPersistentStorage = nullptr;
    CCommandsRegistry*m_pCommandsRegistry = nullptr;


    bool m_bMouseCursorVisible = true;
    
public:
    Application(int argc, char* argv[]);
    ~Application();

    void Run();
    void Terminate() { m_bTerminated = true; }

    static Application * Instance();
    static FileSystem * GetFileSystem();
    static PersistentStorage * GetPersistentStorage();
    static CCommandsRegistry* CommandsRegistry();
    static MainWindow* GetMainWindow() { return Instance()->m_pMainWindow; }

    static void EPICFAIL(const char *format, ...);

    void FlagUpdateStyles() { m_pMainWindow->FlagUpdateStyles(); }

    void         ShowMouseCursor();
    void         HideMouseCursor();
};


#define SDL_ORGANIZATION "QuiteOldOrange"
#define SDL_APP_NAME "ImPFXEditor"