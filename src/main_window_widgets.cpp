#include "imgui_shared.h"
#include "opengl_shared.h"
#include "main_window_widgets.h"

MainWindowWidgets::MainWindowWidgets(MainWindow* pOwner)
{

}


MainWindowWidgets::~MainWindowWidgets()
{

}

void MainWindowWidgets::Render()
{
    RenderMainMenu();
}

void MainWindowWidgets::RenderMainMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("STUB", "SHORTCUT"))
            {
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("About"))
        {
            ImGui::EndMenu();
        }

        m_MainMenuHeight = ImGui::GetWindowHeight();
        ImGui::EndMainMenuBar();
    }
}
