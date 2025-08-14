/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/

#pragma once
#include <list>
#include <stack>
#include <string>
#include <vector>

enum class PopupWindows
{
    LightBaker3KConfig = 0,
    FileDialog,
    SceneScaleDialog,
    ProgramOptions,
    EditGameConfiguration,
    MesasgeBox,
    TexturesBrowser,
    SelectItemFromList
};

class IImGUIPopup
{
protected:
    bool        m_bVisible;
    std::string m_Key;

    virtual void OpenPopup();
    bool         m_bSchedulePopupOpen = false;

    bool m_bPersistent = true;

private:
    PopupWindows m_Id;

    bool m_bDontProcessThisFrame = false;

    void SchedulePopupOpen();
    friend class PopupsManager;

public:
    IImGUIPopup(PopupWindows id);
    virtual ~IImGUIPopup(){};

    virtual bool BeginRendering();
    virtual void EndRendering();

    virtual int  RenderingFlags();
    virtual void Render() = 0;

    void SetVisible(bool bFlag);
    bool IsVisible();

    PopupWindows Id();

    virtual void OnOpen();
    const char * GetKey();

    bool IsPersistent();

    void Show();
    void Close();
};

class PopupsManager
{
    std::list<IImGUIPopup *> m_vPopups;
    std::list<IImGUIPopup *> m_lstOpenedPopups;

    PopupsManager();

public:
    static PopupsManager *Instance();
    ~PopupsManager();

    void ApplyFileDialogSkins();

    void ShowPopup(PopupWindows id);
    void RenderPopups();

    void ShowPopup(IImGUIPopup *pPopup);

    IImGUIPopup *FindPopupByID(PopupWindows id);
};

#include "popup_messagebox.h"