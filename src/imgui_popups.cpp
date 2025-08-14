/*
    LightBaker3000 Frontend project,
    (c) 2022-2025 CrazyRussian
*/

#include "common.h"
#include "imgui_shared.h"
#include "imgui_popups.h"


IImGUIPopup::IImGUIPopup(PopupWindows id)
{
    m_bVisible = false;
    m_Id       = id;
}

bool IImGUIPopup::BeginRendering()
{
    return ImGui::BeginPopupModal(GetKey(), &m_bVisible, RenderingFlags());
}

void IImGUIPopup::EndRendering()
{
    ImGui::PopID();
}

int IImGUIPopup::RenderingFlags()
{
    return ImGuiWindowFlags_AlwaysAutoResize;
}

void IImGUIPopup::Render()
{
}

void IImGUIPopup::SetVisible(bool bFlag)
{
    m_bVisible = bFlag;
}

bool IImGUIPopup::IsVisible()
{
    return m_bVisible;
}

PopupWindows IImGUIPopup::Id()
{
    return m_Id;
}

void IImGUIPopup::OnOpen()
{
}

const char *IImGUIPopup::GetKey()
{
    return m_Key.c_str();
}

bool IImGUIPopup::IsPersistent()
{
    return m_bPersistent;
}

void IImGUIPopup::Show()
{
    PopupsManager::Instance()->ShowPopup(this);
}

void IImGUIPopup::Close()
{
    SetVisible(false);
    ImGui::CloseCurrentPopup();
}

void IImGUIPopup::SchedulePopupOpen()
{
    m_bSchedulePopupOpen = true;
}

void IImGUIPopup::OpenPopup()
{
    if (m_bSchedulePopupOpen)
    {
        ImGui::OpenPopup(GetKey());
        m_bSchedulePopupOpen = false;
    }
}

IImGUIPopup *PopupsManager::FindPopupByID(PopupWindows id)
{
    for (auto &it : m_vPopups)
    {
        if (it->Id() == id)
            return it;
    }

    return nullptr;
}

PopupsManager::PopupsManager()
{
    m_vPopups.clear();
    // m_vPopups.push_back(new LB3kConfigPopup());
    // m_vPopups.push_back(new FileDialog());
    // m_vPopups.push_back(new SceneScaleDialog());
    // m_vPopups.push_back(new OptionsDialog());
    // m_vPopups.push_back(new PopupEditGameconfiguration());
    // m_vPopups.push_back(new TexturesBrowser());
}

PopupsManager *PopupsManager::Instance()
{
    static PopupsManager *sInstance = new PopupsManager;
    return sInstance;
}

PopupsManager::~PopupsManager()
{
    // ClearPointersVector(m_vPopups);

    for (auto &it : m_vPopups)
        delete it;

    m_vPopups.clear();
}

void PopupsManager::ApplyFileDialogSkins()
{
    // ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", ImGui::GetStyleColorVec4(ImGuiCol_Text),
    // std::string(ICON_IGFD_FOLDER)); ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "",
    // ImGui::GetStyleColorVec4(ImGuiCol_PlotLines), std::string(ICON_IGFD_FILE));
}

void PopupsManager::ShowPopup(PopupWindows id)
{
    auto p = FindPopupByID(id);

    assert(p);

    p->SchedulePopupOpen();
    p->OnOpen();
    p->SetVisible(true);

    m_lstOpenedPopups.push_back(p);
}

void PopupsManager::ShowPopup(IImGUIPopup *pPopup)
{
    m_vPopups.push_back(pPopup);

    pPopup->m_bPersistent = false;
    pPopup->SetVisible(true);
    pPopup->SchedulePopupOpen();
    pPopup->m_bDontProcessThisFrame = true;

    m_lstOpenedPopups.push_back(pPopup);
}

void PopupsManager::RenderPopups()
{
    int depth = 0;

    for (auto it = m_lstOpenedPopups.begin(); it != m_lstOpenedPopups.end(); ++it)
    {
        if ((*it)->m_bDontProcessThisFrame)
        {
            (*it)->m_bDontProcessThisFrame = false;
            continue;
        }

        auto ptr = *it;

        if (!ptr->IsVisible())
        {
            m_lstOpenedPopups.erase(it++);

            if (it == m_lstOpenedPopups.end())
                break;

            if (ptr->IsPersistent())
                delete ptr;

            ptr = *it;
        }

        ptr->OpenPopup();

        if (ptr->BeginRendering())
        {
            ImGui::PushID(ptr);
            ptr->Render();
            ImGui::PopID();
            depth++;
        }
    }

    for (int i = 0; i < depth; i++)
        ImGui::EndPopup();

    m_vPopups.remove_if([&](IImGUIPopup *test) { return !test->IsVisible() && !test->IsPersistent(); });
}
