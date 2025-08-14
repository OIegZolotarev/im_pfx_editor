/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/

#include "application.h"
#include "common.h"
#include "text_utils.h"
#include "fs_core.h"

#include "persistent.h"
#include "variant.h"

#include "ui_styles_manager.h"



using namespace UIStyles;

Manager *Manager::Instance()
{
    static Manager *sInstance = new Manager;
    return sInstance;
}

void UIStyles::Manager::SelectNewStyle(size_t index)
{
    if (index < (m_Styles.size()))
    {
        m_Styles[index]->Apply();
    }
    else
    {
        Con_Printf("UIStyles::Manager::SelectNewStyle(): bad style index %d, falling back to style 0\n", (int)index);
    }
}

void Manager::ScheduleStyleUpdateForAllWindows()
{
    Application::Instance()->FlagUpdateStyles();

    // auto &lst = Application::Instance()->GetAllWindows();

    // for (auto &it : lst)
    // {
    //     it->ScheduleImGuiStyleUpdate();
    // }
}

Manager::~Manager()
{
    ClearPointersVector(m_Styles);
}

Manager::Manager()
{
    m_Styles.push_back(new BuiltInStyle(0, "ImGui Dark"));
    m_Styles.push_back(new BuiltInStyle(1, "ImGui Light"));
    m_Styles.push_back(new BuiltInStyle(2, "ImGui Classic"));

    FileData *fd = Application::GetFileSystem()->LoadFile("res/ui/styles_to_load.txt");

    if (!fd)
        return;

    auto lines = TextUtils::SplitTextSimple((const char *)fd->Data(), fd->Length(), '\n');

    for (auto &line : lines)
        m_Styles.push_back(new UserDefinedStyle(line.c_str()));

    fd->UnRef();
}

void Manager::PopulateStylesOption(VariantValue *pOption)
{
    for (size_t i = 0; i < m_Styles.size(); i++)
    {
        auto style = m_Styles[i];
        pOption->AddEnumValue(style->Description(), (int)i);
    }
}

void Manager::ApplyCurrentStyle()
{
    auto setting = Application::GetPersistentStorage()->GetSetting(ApplicationSettings::GUIColorScheme);
    assert(setting);

    int value = setting->GetEnumValue();
    SelectNewStyle(value);
}

const char *StyleDescriptor::Description()
{
    return m_Description.c_str();
}

void StyleDescriptor::Apply()
{
}

void BuiltInStyle::Apply()
{
    switch (m_id)
    {
    case 0:
        ImGui::StyleColorsDark();
        break;
    case 1:
        ImGui::StyleColorsLight();
        break;
    case 2:
        ImGui::StyleColorsClassic();
        break;
    }
}

UserDefinedStyle::UserDefinedStyle(const char *fileName)
{
    FileData *fd = Application::Instance()->GetFileSystem()->LoadFile(fileName);

    if (!fd)
    {
        return;
    }
    try
    {
        std::string    json_data = std::string(std::string_view((char *)fd->Data(), fd->Length()));
        nlohmann::json j         = nlohmann::json::parse(json_data);

        m_Description = j["description"];

        auto items = j["data"];

        m_bValid = true;

        // special iterator member functions for objects
        for (auto &it : items.items())
        {
            auto &record = it.value();

            auto it2 = record.items().begin();

            auto &          key = it2.key();
            nlohmann::json &val = it2.value();

            std::string _r = val["r"];
            std::string _g = val["g"];
            std::string _b = val["b"];
            std::string _a = val["a"];

            float r = std::stof(_r);
            float g = std::stof(_g);
            float b = std::stof(_b);
            float a = std::stof(_a);

            styleColorDescriptor colorDesrc;
            colorDesrc.first  = ColorIdFromString(key.c_str());
            colorDesrc.second = ImVec4(r, g, b, a);

            m_StyleData.push_back(colorDesrc);
        }
    }
    catch (std::exception &e)
    {
        e;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception while parsing style data", e.what(), 0);
        m_bValid = false;
    }

    fd->UnRef();
}

UserDefinedStyle::~UserDefinedStyle()
{
}

ImGuiCol_ UserDefinedStyle::ColorIdFromString(const char *value)
{
    // ������������� ������������ � Notepad++ � ������� ���������,
    // ���� ����� ������ �����������.

    if (!strcmp("ImGuiCol_Text", value))
        return ImGuiCol_Text;
    else if (!strcmp("ImGuiCol_TextDisabled", value))
        return ImGuiCol_TextDisabled;
    else if (!strcmp("ImGuiCol_WindowBg", value))
        return ImGuiCol_WindowBg;
    else if (!strcmp("ImGuiCol_ChildBg", value))
        return ImGuiCol_ChildBg;
    else if (!strcmp("ImGuiCol_PopupBg", value))
        return ImGuiCol_PopupBg;
    else if (!strcmp("ImGuiCol_Border", value))
        return ImGuiCol_Border;
    else if (!strcmp("ImGuiCol_BorderShadow", value))
        return ImGuiCol_BorderShadow;
    else if (!strcmp("ImGuiCol_FrameBg", value))
        return ImGuiCol_FrameBg;
    else if (!strcmp("ImGuiCol_FrameBgHovered", value))
        return ImGuiCol_FrameBgHovered;
    else if (!strcmp("ImGuiCol_FrameBgActive", value))
        return ImGuiCol_FrameBgActive;
    else if (!strcmp("ImGuiCol_TitleBg", value))
        return ImGuiCol_TitleBg;
    else if (!strcmp("ImGuiCol_TitleBgActive", value))
        return ImGuiCol_TitleBgActive;
    else if (!strcmp("ImGuiCol_TitleBgCollapsed", value))
        return ImGuiCol_TitleBgCollapsed;
    else if (!strcmp("ImGuiCol_MenuBarBg", value))
        return ImGuiCol_MenuBarBg;
    else if (!strcmp("ImGuiCol_ScrollbarBg", value))
        return ImGuiCol_ScrollbarBg;
    else if (!strcmp("ImGuiCol_ScrollbarGrab", value))
        return ImGuiCol_ScrollbarGrab;
    else if (!strcmp("ImGuiCol_ScrollbarGrabHovered", value))
        return ImGuiCol_ScrollbarGrabHovered;
    else if (!strcmp("ImGuiCol_ScrollbarGrabActive", value))
        return ImGuiCol_ScrollbarGrabActive;
    else if (!strcmp("ImGuiCol_CheckMark", value))
        return ImGuiCol_CheckMark;
    else if (!strcmp("ImGuiCol_SliderGrab", value))
        return ImGuiCol_SliderGrab;
    else if (!strcmp("ImGuiCol_SliderGrabActive", value))
        return ImGuiCol_SliderGrabActive;
    else if (!strcmp("ImGuiCol_Button", value))
        return ImGuiCol_Button;
    else if (!strcmp("ImGuiCol_ButtonHovered", value))
        return ImGuiCol_ButtonHovered;
    else if (!strcmp("ImGuiCol_ButtonActive", value))
        return ImGuiCol_ButtonActive;
    else if (!strcmp("ImGuiCol_Header", value))
        return ImGuiCol_Header;
    else if (!strcmp("ImGuiCol_HeaderHovered", value))
        return ImGuiCol_HeaderHovered;
    else if (!strcmp("ImGuiCol_HeaderActive", value))
        return ImGuiCol_HeaderActive;
    else if (!strcmp("ImGuiCol_Separator", value))
        return ImGuiCol_Separator;
    else if (!strcmp("ImGuiCol_SeparatorHovered", value))
        return ImGuiCol_SeparatorHovered;
    else if (!strcmp("ImGuiCol_SeparatorActive", value))
        return ImGuiCol_SeparatorActive;
    else if (!strcmp("ImGuiCol_ResizeGrip", value))
        return ImGuiCol_ResizeGrip;
    else if (!strcmp("ImGuiCol_ResizeGripHovered", value))
        return ImGuiCol_ResizeGripHovered;
    else if (!strcmp("ImGuiCol_ResizeGripActive", value))
        return ImGuiCol_ResizeGripActive;
    else if (!strcmp("ImGuiCol_Tab", value))
        return ImGuiCol_Tab;
    else if (!strcmp("ImGuiCol_TabHovered", value))
        return ImGuiCol_TabHovered;
    else if (!strcmp("ImGuiCol_TabActive", value))
        return ImGuiCol_TabActive;
    else if (!strcmp("ImGuiCol_TabUnfocused", value))
        return ImGuiCol_TabUnfocused;
    else if (!strcmp("ImGuiCol_TabUnfocusedActive", value))
        return ImGuiCol_TabUnfocusedActive;
    else if (!strcmp("ImGuiCol_DockingPreview", value))
        return ImGuiCol_DockingPreview;
    else if (!strcmp("ImGuiCol_DockingEmptyBg", value))
        return ImGuiCol_DockingEmptyBg;
    else if (!strcmp("ImGuiCol_PlotLines", value))
        return ImGuiCol_PlotLines;
    else if (!strcmp("ImGuiCol_PlotLinesHovered", value))
        return ImGuiCol_PlotLinesHovered;
    else if (!strcmp("ImGuiCol_PlotHistogram", value))
        return ImGuiCol_PlotHistogram;
    else if (!strcmp("ImGuiCol_PlotHistogramHovered", value))
        return ImGuiCol_PlotHistogramHovered;
    else if (!strcmp("ImGuiCol_TableHeaderBg", value))
        return ImGuiCol_TableHeaderBg;
    else if (!strcmp("ImGuiCol_TableBorderStrong", value))
        return ImGuiCol_TableBorderStrong;
    else if (!strcmp("ImGuiCol_TableBorderLight", value))
        return ImGuiCol_TableBorderLight;
    else if (!strcmp("ImGuiCol_TableRowBg", value))
        return ImGuiCol_TableRowBg;
    else if (!strcmp("ImGuiCol_TableRowBgAlt", value))
        return ImGuiCol_TableRowBgAlt;
    else if (!strcmp("ImGuiCol_TextSelectedBg", value))
        return ImGuiCol_TextSelectedBg;
    else if (!strcmp("ImGuiCol_DragDropTarget", value))
        return ImGuiCol_DragDropTarget;
    else if (!strcmp("ImGuiCol_NavHighlight", value))
        return ImGuiCol_NavHighlight;
    else if (!strcmp("ImGuiCol_NavWindowingHighlight", value))
        return ImGuiCol_NavWindowingHighlight;
    else if (!strcmp("ImGuiCol_NavWindowingDimBg", value))
        return ImGuiCol_NavWindowingDimBg;
    else if (!strcmp("ImGuiCol_ModalWindowDimBg", value))
        return ImGuiCol_ModalWindowDimBg;

    return ImGuiCol_::ImGuiCol_COUNT;
}

void UserDefinedStyle::Apply()
{
    if (!m_bValid)
    {
        ImGui::StyleColorsClassic();
        return;
    }

    ImVec4 *colors = ImGui::GetStyle().Colors;

    for (auto &items : m_StyleData)
    {
        colors[items.first] = items.second;
    }
}
