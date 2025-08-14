/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/

#include "application.h"
#include "imgui_shared.h"
#include "popup_options_window.h"

#include "common.h"
#include "imgui_popups.h"

#include "scene_camera_controller.h"
#include "ui_options_pages.h"
#include "ui_styles_manager.h"

#include "persistent.h"



using namespace ProgramOptions;

extern uiOptionPage_t g_OptionsPages[(int)OptionsPage::Total];

void RegisterOptions()
{
    VariantValue *opt = nullptr;

    BeginOptionPage(OptionsPage::General, "General");

    AddGroup("Scene");
    //AddOption(ApplicationSettings::RebakeSceneAfterChanges, "Auto-rebake after changes", PropertiesTypes::Bool);
    AddOption(ApplicationSettings::ShowGround, "Display ground", PropertiesTypes::Bool);
    // AddOption(ApplicationSettings::SelectedObjectColor, "Selection color", PropertiesTypes::ColorRGB);
    // AddOption(ApplicationSettings::SelectionBoxColor, "Selection box color", PropertiesTypes::ColorRGB);

    AddGroup("Background");
    AddOption(ApplicationSettings::UseGradientBackground, "Use gradient", PropertiesTypes::Bool);
    AddOption(ApplicationSettings::BackgroundColor1, "Color 1", PropertiesTypes::ColorRGB);
    AddOption(ApplicationSettings::BackgroundColor2, "Color 2 (gradient)", PropertiesTypes::ColorRGB);

    BeginOptionPage(OptionsPage::Camera, "Camera");

    // Порядок перечислений должен совпадать с основным перечислением!!
    opt = AddOption(ApplicationSettings::CameraControlScheme, "Camera control scheme", PropertiesTypes::Enum);
    opt->AddEnumValue("Valve Hammer Editor", (int)CameraControlScheme::ValveHammerEditor);
    opt->AddEnumValue("Blender", (int)CameraControlScheme::Blender);
    opt->AddEnumValue("Blender (touchpad)", (int)CameraControlScheme::BlenderTouchpad);

    opt = AddOption(ApplicationSettings::CameraMouseSensivityRotating, "Mouse sensitivity (rotation)",
                    PropertiesTypes::Float);
    opt->SetNumericalLimits(0.1f, 10.f);

    opt = AddOption(ApplicationSettings::CameraMouseSensivityPaning, "Mouse sensitivity (pan)", PropertiesTypes::Float);
    opt->SetNumericalLimits(0.1f, 10.f);

    opt =
        AddOption(ApplicationSettings::CameraMouseSensivityZooming, "Mouse sensitivity (zoom)", PropertiesTypes::Float);
    opt->SetNumericalLimits(0.1f, 10.f);

    AddGroup("Movement");

    AddOption(ApplicationSettings::CameraMovementSpeed, "Speed", PropertiesTypes::Float);

    opt = AddOption(ApplicationSettings::CameraAccel, "Acceleration", PropertiesTypes::Float);
    // opt->SetNumericalLimits(0, 300);

    opt = AddOption(ApplicationSettings::CameraDecel, "Deceleration", PropertiesTypes::Float);
    // opt->SetNumericalLimits(0, 300);

    AddGroup("Parameters");

    opt = AddOption(ApplicationSettings::CameraFov, "Field of view", PropertiesTypes::Float);
    opt->SetNumericalLimits(1.f, 179.f);

    opt = AddOption(ApplicationSettings::CameraZNear, "Near plane", PropertiesTypes::Float);
    opt->SetNumericalLimits(0.001f, 10.f);

    opt = AddOption(ApplicationSettings::CameraZFar, "Far plane", PropertiesTypes::Float);
    opt->SetNumericalLimits(10.f, 1000000.f);

    BeginOptionPage(OptionsPage::Appearence, "Appeareance");

    AddGroup("Appearance");
    opt = AddOption(ApplicationSettings::GUIColorScheme, "UI colors", PropertiesTypes::Enum);
    UIStyles::Manager::Instance()->PopulateStylesOption(opt);

    AddGroup("Grid");
    opt = AddOption(ApplicationSettings::GridAxisColor, "Axis color", PropertiesTypes::ColorRGB);
    opt = AddOption(ApplicationSettings::GridMainColor, "Main color", PropertiesTypes::ColorRGB);
    opt = AddOption(ApplicationSettings::Grid64thLineColor, "64th lines color", PropertiesTypes::ColorRGB);
    opt = AddOption(ApplicationSettings::Grid1024thLineColor, "1024th lines color", PropertiesTypes::ColorRGB);
    opt = AddOption(ApplicationSettings::GridCustomColor, "Custom color", PropertiesTypes::ColorRGB);
    opt = AddOption(ApplicationSettings::GridCustomStep, "Highlight every n'th line", PropertiesTypes::Int);
}

OptionsDialog::OptionsDialog() : IImGUIPopup(PopupWindows::ProgramOptions)
{
    m_Key = "Preferences";

    
}

OptionsDialog::~OptionsDialog()
{    
}

void OptionsDialog::Render()
{
    if (!RenderHeader())
        return;

    ImVec2 size(-FLT_MIN, -FLT_MIN);

    uiOptionPage_t *pageToRender = nullptr;

    if (ImGui::BeginTabBar("###Category"))
    {
        for (int i = 0; i < ARRAYSIZE(g_OptionsPages); i++)
        {
            auto page = &g_OptionsPages[i];

            if (page->selected)
                pageToRender = page;

            // if (ImGui::Selectable(page->pageDescription.c_str(), &page->selected, ImGuiSelectableFlags_None))
            if (ImGui::BeginTabItem(page->pageDescription.c_str()))
            {
                pageToRender = page;

                for (int k = 0; k < ARRAYSIZE(g_OptionsPages); k++)
                {
                    if (k == i)
                        continue;
                    g_OptionsPages[k].selected = false;
                }

                ImGui::EndTabItem();
            }
        }

        if (ImGui::BeginTabItem("Game configurations"))
        {
            if (ImGui::BeginChild("ChildId", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())))
            {
                RenderGameConfigurationsPage();
                ImGui::EndChild();
            }

            ImGui::EndTabItem();
        }

        // ImGui::EndListBox();
        ImGui::EndTabBar();
    }

    if (pageToRender)
    {
        if (ImGui::BeginChild("ChildId", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())))
        {
            RenderOptionsPages(pageToRender);
        }

        ImGui::EndChild();
    }

    RenderFooter();
}

void OptionsDialog::RenderFooter()
{
    // ImGui::Separator();

    if (ImGui::Button("OK"))
        OnOkPressed();

    ImGui::SameLine();

    if (ImGui::Button("Cancel"))
        OnCancelPressed();
}

bool OptionsDialog::RenderHeader()
{
    //     if (!ImGui::BeginPopupModal(key, &m_bVisible, ImGuiWindowFlags_NoResize))
    //         return false;

    ImGui::SetWindowSize(ImVec2(600, 400));

    return true;
}

void OptionsDialog::RenderOptionsPages(ProgramOptions::uiOptionPage_t *page)
{
    if (ImGui::BeginChildFrame(53, ImVec2(0, 0)))
    {
        if (ImGui::BeginTable("split", 2, ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("One", ImGuiTableColumnFlags_WidthStretch, 50);
            ImGui::TableSetupColumn("Two", ImGuiTableColumnFlags_WidthStretch, 50);

            int i = 0;

            for (auto &it : page->items)
            {
                ImGui::PushID(i++); // Use field index as identifier.

                // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                ImGui::TableNextRow();

                it->RenderImGUI();

                ImGui::PopID();
            }

            ImGui::EndTable();
        }
        ImGui::EndChildFrame();
    }
}

void OptionsDialog::RenderGameConfigurationsPage()
{
    if (ImGui::BeginChildFrame(53, ImVec2(0, 0)))
    {
        ImGui::SeparatorText("Registered configurations:");
        // m_pGameConfigurationsView->RenderGui();

        // auto defaultConfiguration = GameConfigurationsManager::Instance()->GetDefaultGameConfiguration();

        // auto ptr = defaultConfiguration.lock();

        // if (ptr)
        //     ImGui::Text("Default configuration: %s", ptr->Description());
        // else
        //     ImGui::Text("Default configuration is not set");

        ImGui::Text("LEL");

        ImGui::EndChildFrame();
    }
}

void OptionsDialog::OnOpen()
{
}

int OptionsDialog::RenderingFlags()
{
    return ImGuiWindowFlags_NoResize;
}

void OptionsDialog::OnOkPressed()
{
    m_bVisible = false;
}

void OptionsDialog::OnCancelPressed()
{
    m_bVisible = false;
}

