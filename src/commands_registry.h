/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/

#pragma once

#include "common_icons.h"
#include "gl_texture.h"

enum class GlobalCommands
{
    ExitApplication = 0,
    
    // Grid control
    IncreaseGridStep,
    DecreaseGridStep,
    // Camera
    CameraLookFromTop,
    CameraLookFromBottom,
    CameraLookFromLeft,
    CameraLookFromRight,
    CameraLookFromFront,
    CameraLookFromBack,
    

    None
};

typedef std::function<void()> pfnCommandCallback;

#define CMD_ON_MAINTOOLBAR (1 << 0)
#define CMD_ONLY_ICON      (1 << 1)
#define CMD_ONLY_TEXT      (1 << 2)

class CCommand
{
    char m_szDescription[32];
    char m_szKeyStroke[32];

    int m_rKeys[6 + 3] = {-1};

    int                m_iFlags;
    GLTexture *        m_pIcon;
    CommonIcons        m_CommonIcon;
    GlobalCommands     m_eCommandId;
    pfnCommandCallback m_pfnCallback;

    void ParseKeyStroke();

public:
    CCommand(GlobalCommands id, const char *description, const char *keyStroke, GLTexture *icon, int flags,
             pfnCommandCallback callback);

    CCommand(){};

    void SetId(GlobalCommands id);
    void SetDescription(const char *descr);
    void SetKeyStroke(const char *keyStroke);

    void SetCustomIcon(GLTexture *pIcon);
    void SetCommonIcon(CommonIcons icon);

    void SetFlags(int flags);
    void SetCallback(pfnCommandCallback callback);

    void           Execute();
    int            Flags();
    void           RenderImGUI(int size = 32);
    GlobalCommands GetId();
    const char *   GetDescription();

    bool        IsKeystrokeActive(const uint8_t *kbState);
    const char *GetShortcutDescription();
    CommonIcons GetCommonIcon();
};

class CCommandsRegistry
{
    std::vector<size_t>     m_vecMainToolbarCommands;
    std::vector<CCommand *> m_vecCommandDescriptor;

    void RegisterCameraTurningCommands();

public:
    CCommandsRegistry();
    ~CCommandsRegistry();

    void RegisterCommand(CCommand *pCommand);

    std::vector<size_t> &GetMainToolbarCommands();
    CCommand *           GetCommandByInternalIndex(int index);
    CCommand *           FindCommandByGlobalId(GlobalCommands id);

    void RenderCommandAsMenuItem(GlobalCommands cmdId, bool checked = false);

    bool OnKeyDown();

    void InitializeAllCommands();

    
};
