#include "application.h"
#include "commands_registry.h"

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
    PopupsManager::Instance()->RenderPopups();
}

void MainWindowWidgets::RenderMainMenu()
{
    auto registry = Application::CommandsRegistry();
    #define COMMAND_ITEM registry->RenderCommandAsMenuItem

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            COMMAND_ITEM(GlobalCommands::ExitApplication);

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
