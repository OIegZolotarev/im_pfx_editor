/*
    LightBaker3000 Frontend project,
    (c) 2024 CrazyRussian
*/

#pragma once

#include <functional>

enum class MessageBoxIcons
{
    Error,
    Notification,
    Warning,
    Question,
    None
};

enum class MessageBoxButtons
{
    Ok,
    Cancel,
    Yes,
    No
};

#define MSG_BOX_OK (1<<0)
#define MSG_BOX_CANCEL (1<<1)
#define MSG_BOX_YES (1<<2)
#define MSG_BOX_NO (1<<3)

typedef std::function<void(MessageBoxButtons)> pfnMessageBoxCallback;


class PopupMessageBox: public IImGUIPopup
{
    std::string m_Message;
    std::string m_Title;
    MessageBoxIcons m_Icon;

    int m_iButtonsFlags;
    pfnMessageBoxCallback m_Callback;

public:
    PopupMessageBox();
    ~PopupMessageBox();

    void Render() override;

    void SetMessage(const char *message);
    void SetTitle(const char *title);
    void SetIcon(MessageBoxIcons icon);
    void SetButtons(int flags);
    void SetCallback(pfnMessageBoxCallback callback);

    int RenderingFlags() override;
    void Show();

    bool BeginRendering() override;
};
