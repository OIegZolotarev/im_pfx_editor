/*
    LightBaker3000 Frontend project,
    (c) 2024 CrazyRussian
*/

#include "../application.h"
#include "../common.h"

#include "imgui_popups.h"
#include "imgui_helpers.h"
#include "popup_messagebox.h"



PopupMessageBox::PopupMessageBox() : IImGUIPopup(PopupWindows::MesasgeBox)
{
    m_Icon        = MessageBoxIcons::None;
    m_bPersistent = false;
}

PopupMessageBox::~PopupMessageBox()
{
}

void PopupMessageBox::Render()
{
    if (ImGui::BeginChild("MessageBoxContent", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())))
    {
        CommonIcons icon = CommonIcons::MessageBoxInfo;

        switch (m_Icon)
        {
        case MessageBoxIcons::Error:
            icon = CommonIcons::MessageBoxError;
            break;
        case MessageBoxIcons::Notification:
            icon = CommonIcons::MessageBoxInfo;
            break;
        case MessageBoxIcons::Warning:
            icon = CommonIcons::MessageBoxWarning;
            break;
        case MessageBoxIcons::Question:
            icon = CommonIcons::MessageBoxHelp;
            break;
        case MessageBoxIcons::None:
            break;
        default:
            break;
        
        }

        ImGui::Spacing();
        ImGuiHelpers::DisplayCommonIcon(icon, 48);
        ImGui::SameLine();
        ImGui::Text(m_Message.c_str());
    }

    ImGui::EndChild();

    auto displayButton = [&](int flag, const char *title, MessageBoxButtons id) {
        if (m_iButtonsFlags & flag)
        {
            if (ImGui::Button(title))
            {
                SetVisible(false);
                ImGui::CloseCurrentPopup();

                if (m_Callback)
                    m_Callback(id);

                
            }

            ImGui::SameLine();
        }
    };

    displayButton(MSG_BOX_OK, "Ok", MessageBoxButtons::Ok);
    displayButton(MSG_BOX_YES, "Yes", MessageBoxButtons::Yes);
    displayButton(MSG_BOX_NO, "No", MessageBoxButtons::No);
    displayButton(MSG_BOX_CANCEL, "Cancel", MessageBoxButtons::Cancel);
}

void PopupMessageBox::SetMessage(const char *message)
{
    m_Message = message;
}

void PopupMessageBox::SetTitle(const char *title)
{
    m_Title = title;
    m_Key   = title;
}

void PopupMessageBox::SetIcon(MessageBoxIcons icon)
{
    m_Icon = icon;
}

void PopupMessageBox::SetButtons(int flags)
{
    m_iButtonsFlags = flags;
}

void PopupMessageBox::SetCallback(pfnMessageBoxCallback callback)
{
    m_Callback = callback;
}

int PopupMessageBox::RenderingFlags()
{
    return ImGuiWindowFlags_NoResize;
}

void PopupMessageBox::Show()
{
    PopupsManager::Instance()->ShowPopup(this);
}

bool PopupMessageBox::BeginRendering()
{
    ImGui::SetNextWindowSize(ImVec2(350, 150));
    ImGui::SetNextWindowPos(ImGui::GetWindowViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    return IImGUIPopup::BeginRendering();
}
