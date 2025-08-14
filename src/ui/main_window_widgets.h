#pragma once

#include "main_window.h"

class MainWindowWidgets
{
    float m_MainMenuHeight = 0;
public:
    MainWindowWidgets(MainWindow* pOwner);
    ~MainWindowWidgets();

    void Render();
    void RenderMainMenu();
};