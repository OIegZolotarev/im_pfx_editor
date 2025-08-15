/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/

#include "application.h"
#include "commands_registry.h"

#include "common.h"
#include "imgui_shared.h"
#include "imgui_helpers.h"

#include "text_utils.h"


#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

CCommandsRegistry::CCommandsRegistry()
{
    m_vecCommandDescriptor.clear();

    
}

CCommandsRegistry::~CCommandsRegistry()
{
    ClearPointersVector(m_vecCommandDescriptor);
}

void CCommandsRegistry::RegisterCommand(CCommand *pCommand)
{
    m_vecCommandDescriptor.push_back(pCommand);

    if (pCommand->Flags() & CMD_ON_MAINTOOLBAR)
        m_vecMainToolbarCommands.push_back(m_vecCommandDescriptor.size() - 1);
}

std::vector<size_t> &CCommandsRegistry::GetMainToolbarCommands()
{
    return m_vecMainToolbarCommands;
}

CCommand *CCommandsRegistry::GetCommandByInternalIndex(int index)
{
    return m_vecCommandDescriptor[index];
}

CCommand *CCommandsRegistry::FindCommandByGlobalId(GlobalCommands id)
{
    for (auto &it : m_vecCommandDescriptor)
    {
        if (it->GetId() == id)
            return it;
    }

    return 0;
}

void CCommandsRegistry::RenderCommandAsMenuItem(GlobalCommands cmdId, bool checked)
{
    CCommand *cmd = FindCommandByGlobalId(cmdId);
    assert(cmd);

    bool tmp2 = checked;

    if (ImGui::MenuItem(cmd->GetDescription(), cmd->GetShortcutDescription(), &tmp2))
    {
        cmd->Execute();
    }
}

bool CCommandsRegistry::OnKeyDown()
{
    auto kbState = SDL_GetKeyboardState(nullptr);

    for (auto &cmd : m_vecCommandDescriptor)
    {
        if (cmd->IsKeystrokeActive(kbState))
        {
            cmd->Execute();
            return true;
        }
    }

    return false;
}

void CCommandsRegistry::InitializeAllCommands()
{

    CCommand *newCommand;

    newCommand = new CCommand;
    newCommand->SetId(GlobalCommands::ExitApplication);
    newCommand->SetDescription("Exit");
    newCommand->SetKeyStroke(nullptr);        
    newCommand->SetCallback([&]() {
        Application::Instance()->Terminate();
    });
    Application::CommandsRegistry()->RegisterCommand(newCommand);


    RegisterCameraTurningCommands();

}

void CCommandsRegistry::RegisterCameraTurningCommands()
{
    struct cmdDescriptor_s
    {
        GlobalCommands id;
        glm::vec3      angles;
        const char *   descr;
        const char *   key;
    };

    cmdDescriptor_s commands[] = {
        // clang-format off
        {GlobalCommands::CameraLookFromFront  , {0   , 90  , 0} , "Front"  , "Keypad 8"} ,
        {GlobalCommands::CameraLookFromBack   , {0   , 270 , 0} , "Back"   , "Keypad 2"} ,
        {GlobalCommands::CameraLookFromLeft   , {0   , 180 , 0} , "Left"   , "Keypad 4"} ,
        {GlobalCommands::CameraLookFromRight  , {0   , 0   , 0} , "Right"  , "Keypad 6"} ,
        {GlobalCommands::CameraLookFromTop    , {-90 , 0   , 0} , "Top"    , "Keypad 7"} ,
        {GlobalCommands::CameraLookFromBottom , {90  , 0   , 0} , "Bottom" , "Keypad 3"} ,
        // clang-format on
    };

    for (auto &it : commands)
    {
        CCommand *newCommand = new CCommand;
        newCommand->SetId(it.id);
        newCommand->SetDescription(it.descr);
        newCommand->SetKeyStroke(it.key);
        newCommand->SetCommonIcon(CommonIcons::None);
        newCommand->SetFlags(0);
        newCommand->SetCallback([=]() {
            // auto viewport = ViewportsOrchestrator::Instance()->GetFocusedViewport();
            // if (!viewport)
            //     return;

            // viewport->GetCamera()->ExecuteTransition(it.angles);
        });

        RegisterCommand(newCommand);
    }
}

void CCommand::ParseKeyStroke()
{
    for (auto &it : m_rKeys)
        it = -1;

    if (*m_szKeyStroke)
    {
        auto items = TextUtils::SplitTextSimple(m_szKeyStroke, strlen(m_szKeyStroke), '-');

        auto addKey = [&](int keyId) -> bool {
            int i = 0;
            for (int i = 0; i < ARRAY_SIZE(m_rKeys); i++)
            {
                if (m_rKeys[i] == -1)
                {
                    m_rKeys[i] = keyId;
                    return true;
                }
            }

            return false;
        };

        auto it = items.begin();

        while (it != items.end())
        {
            bool r = false;

            auto scanCode = SDL_GetScancodeFromName((*it).c_str());

            if (scanCode == SDLK_UNKNOWN)
            {
                auto err = SDL_GetError();
                Application::EPICFAIL("Bad shortcut for command \"%s\" - unknown key %s", m_szDescription,
                                      (*it).c_str());
            }

            r = addKey(scanCode);

            if (!r)
                Application::EPICFAIL("Bad shortcut for command \"%s\" - key stroke is too long", m_szDescription);

            it++;
        }
    }
}

CCommand::CCommand(GlobalCommands id, const char *description, const char *keyStroke, GLTexture *icon, int flags,
                   pfnCommandCallback callback)
    : m_eCommandId(id), m_pIcon(icon), m_iFlags(flags), m_pfnCallback(callback)
{
    strlcpy(m_szDescription, description, sizeof(m_szDescription));

    if (keyStroke)
        strlcpy(m_szKeyStroke, keyStroke, sizeof(m_szDescription));
    else
        *m_szKeyStroke = 0;

    ParseKeyStroke();
}

void CCommand::SetId(GlobalCommands id)
{
    m_eCommandId = id;
}

void CCommand::SetDescription(const char *descr)
{
    strlcpy(m_szDescription, descr, sizeof(m_szDescription));
}

void CCommand::SetKeyStroke(const char *keyStroke)
{
    if (!keyStroke)
        *m_szKeyStroke = 0;
    else
        strlcpy(m_szKeyStroke, keyStroke, sizeof(m_szKeyStroke));

    ParseKeyStroke();
}

void CCommand::SetCustomIcon(GLTexture *pIcon)
{
    m_pIcon = pIcon;
}

void CCommand::SetCommonIcon(CommonIcons icon)
{
    m_CommonIcon = icon;
}

void CCommand::SetFlags(int flags)
{
    m_iFlags = flags;
}

void CCommand::SetCallback(pfnCommandCallback callback)
{
    m_pfnCallback = callback;
}

void CCommand::Execute()
{
    m_pfnCallback();
}

int CCommand::Flags()
{
    return m_iFlags;
}

void CCommand::RenderImGUI(int size)
{
    // ImGUI quirks
#pragma warning(disable : 4312)
    ImGui::SameLine();

    if (m_iFlags & CMD_ONLY_ICON)
    {
        if (ImGui::ImageButton(m_szDescription, (ImTextureID)m_pIcon->GLTextureNum(), ImVec2(size, size)))
            Execute();
    }
    else
    {
        auto style   = ImGui::GetStyle();
        auto bgColor = style.Colors[ImGuiCol_Button];

        // if (ImGui::ImageButton(m_szDescription, (ImTextureID)m_pIcon->gl_texnum, ImVec2(size, size)))
        if (ImGuiHelpers::ImageButtonWithText((ImTextureID)m_pIcon->GLTextureNum(), m_szDescription, ImVec2(size, size),
                                              ImVec2(1, 1), ImVec2(0, 0), style.FramePadding.x, bgColor,
                                              ImVec4(1, 1, 1, 1)))
        {
            Execute();
        }
    }
#pragma warning(restore : 4312)
}

GlobalCommands CCommand::GetId()
{
    return m_eCommandId;
}

const char *CCommand::GetDescription()
{
    return m_szDescription;
}

bool CCommand::IsKeystrokeActive(const uint8_t *kbState)
{
    if (m_rKeys[0] == -1) // No hotkey assigned
        return false;

    bool bValidStroke = true;

    for (auto &key : m_rKeys)
    {
        if (key == -1) // Reached end of key combo
            break;

        if (!kbState[key])
        {
            bValidStroke = false;
            break;
        }
    }

    return bValidStroke;
}

const char *CCommand::GetShortcutDescription()
{
    return m_szKeyStroke;
}

CommonIcons CCommand::GetCommonIcon()
{
    return m_CommonIcon;
}
