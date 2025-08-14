#include "imgui_shared.h"
#include "opengl_shared.h"
#include "main_window_widgets.h"

#include "imgui_popups.h"

MainWindowWidgets::MainWindowWidgets(MainWindow *pOwner)
{
}

MainWindowWidgets::~MainWindowWidgets()
{
}

void MainWindowWidgets::Render()
{
    RenderMainMenu();
    PopupsManager::Instance()->RenderPopups();
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

            if (ImGui::MenuItem("Program options", ""))
            {
                PopupsManager::Instance()->ShowPopup(PopupWindows::ProgramOptions);
            }

            if (ImGui::MenuItem("Test popup", ""))
            {
                PopupMessageBox *pMessageBox = new PopupMessageBox;
                pMessageBox->SetTitle("ATTENTION");
                pMessageBox->SetMessage("THANK YOU FOR YOUR ATTENTION");
                pMessageBox->SetIcon(MessageBoxIcons::Warning);
                pMessageBox->SetButtons(MSG_BOX_OK);

                pMessageBox->Show();
            }

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
