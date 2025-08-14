/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/

#pragma once
#include "common_resources.h"

#define LV_TOOLBAR_ICON_SIZE 13

enum class SortDirection
{
    Ascending = 0,
    Descending
};


#define LV_DISABLE_ADD_ITEMS    (1 << 0)
#define LV_DISABLE_REMOVE_ITEMS (1 << 1)
#define LV_DISABLE_EDIT_ITEMS   (1 << 2)
#define LV_DISABLE_MOVE_ITEMS   (1 << 3)
#define LV_DISABLE_SORT_ITEMS   (1 << 4)
#define LV_HIDE_CAPTIONS        (1 << 5)

class IListBinder
{
  public:
    virtual ~IListBinder() = default;

    virtual void AddNewItem()                 = 0;
    virtual void RemoveItem(size_t index)     = 0;
    virtual void RemoveSelectedItem()         = 0;
    virtual void MoveItemUp()                 = 0;
    virtual void MoveItemDown()               = 0;
    virtual void OpenItemEditor()             = 0;
    virtual void SortItems(SortDirection dir) = 0;

    virtual bool IsEmpty()                = 0;
    virtual void ResetIterator()          = 0;
    virtual const char *ItemDescription() = 0;
    virtual bool IsItemSelected()         = 0;
    virtual void SetSelectedItem()        = 0;
    virtual bool NextItem()               = 0;

    virtual void RenderExtraCommands(){};
};

class ListViewEx
{
    IListBinder *m_pListBinder;
    unsigned int m_Flags;

    void RenderToolbar();

    
enum class ToolbarButtons
    {
        Add = 0,
        Remove,
        Edit,
        MoveUp,
        MoveDown,
        SortAsc,
        SortDesc
    };

  public:
    ListViewEx(IListBinder *pBinder, unsigned int flags);
    ~ListViewEx();
    void RenderGui();

  private:
    void OnToolBarItemClicked(ToolbarButtons btn);
};