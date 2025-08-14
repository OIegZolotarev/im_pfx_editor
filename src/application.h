/*
    ImPFXEditor, (c) CrazyRussian, 2025
*/

#pragma once

#include "ui/main_window.h"

class FileSystem;

class Application
{      
    MainWindow * m_pMainWindow = nullptr;
    bool m_bTerminated = false;

    FileSystem * m_pFileSystem;

public:
    Application(int argc, char* argv[]);
    ~Application();

    void Run();
    void Terminate() { m_bTerminated = true; }

    static Application * Instance();
    static FileSystem * GetFileSystem();

    static void EPICFAIL(const char *format, ...);

};


#define SDL_ORGANIZATION "QuiteOldOrange"
#define SDL_APP_NAME "ImPFXEditor"