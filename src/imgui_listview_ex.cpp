/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/


#include "application.h"
#include "common.h"
#include "common_resources.h"
#include "imgui_helpers.h"
#include "imgui_listview_ex.h"

void ListViewEx::RenderToolbar()
{
    constexpr float iconsSize = LV_TOOLBAR_ICON_SIZE;

    static const char *labels[]   = {"Add", "Remove", "Edit", "Move up", "Move down", "Sort asc.", "Sort desc."};
    static const char *noLabels[] = {"###Add",       "###Remove",     "##Edit",     "###Move up",
                                     "###Move down", "###Sort desc.", "##Sort asc."};

    const char **labelsPtr;

    if (m_Flags & LV_HIDE_CAPTIONS)
        labelsPtr = noLabels;
    else
        labelsPtr = labels;

    ImVec4 tinting[] = {{0, 1, 0, 1}, {1, 0, 0, 1}, {1, 1, 0, 1}, {0, 0, 1, 1},
                        {0, 0, 1, 1}, {1, 0, 1, 1}, {1, 0, 1, 1}};

    size_t startingIcon = (int)CommonTextures::ListAdd;
    size_t endingIcon   = (int)CommonTextures::ListSortDesc;
    size_t iconsCount   = endingIcon - startingIcon;

    bool nextLine     = false;
    bool visibility[] = {
        !(m_Flags & LV_DISABLE_ADD_ITEMS),  !(m_Flags & LV_DISABLE_REMOVE_ITEMS), !(m_Flags & LV_DISABLE_EDIT_ITEMS),
        !(m_Flags & LV_DISABLE_MOVE_ITEMS), !(m_Flags & LV_DISABLE_MOVE_ITEMS),   !(m_Flags & LV_DISABLE_SORT_ITEMS),
        !(m_Flags & LV_DISABLE_SORT_ITEMS),
    };

    // Calculate which icon will be rendered last, so we can properly not call "SameLine" on it
    size_t lastIcon = 0;

    for (size_t i = 0; i <= iconsCount; i++)
    {
        if (visibility[i])
            lastIcon = i;
    }

    for (size_t i = 0; i <= iconsCount; i++)
    {
        if (visibility[i])
        {
            if (ImGuiHelpers::ButtonWithCommonTextureIcon(CommonTextures(startingIcon + i), labelsPtr[i], iconsSize, tinting[i]))
                OnToolBarItemClicked(ToolbarButtons(i));

            nextLine = true;
        }

        // Last one should not call "SameLine"
        if (nextLine && i != lastIcon)
        {
            ImGui::SameLine();
        }

        nextLine = false;
    }

    m_pListBinder->RenderExtraCommands();
}

ListViewEx::ListViewEx(IListBinder *pBinder, unsigned int flags)
{
    m_Flags       = flags;
    m_pListBinder = pBinder;
}

ListViewEx::~ListViewEx()
{
    delete m_pListBinder;
}

void ListViewEx::RenderGui()
{
    ImGui::PushID(this);

    RenderToolbar();
    // Stretch to fit
    ImGui::SetNextItemWidth(-1);

    if (ImGui::BeginListBox("###RegisteredConfigurations"))
    {
        m_pListBinder->ResetIterator();

        if (!m_pListBinder->IsEmpty())
        {
            do
            {
                const char *desc = m_pListBinder->ItemDescription();
                bool bSelected   = m_pListBinder->IsItemSelected();

                if (ImGui::Selectable(desc, bSelected))
                {
                    if (bSelected)
                        m_pListBinder->OpenItemEditor();
                    else
                        m_pListBinder->SetSelectedItem();
                }

            } while (m_pListBinder->NextItem());
        }

        ImGui::EndListBox();
    }

    ImGui::PopID();
}

void ListViewEx::OnToolBarItemClicked(ToolbarButtons btn)
{
    switch (btn)
    {
    case ToolbarButtons::Add:
        m_pListBinder->AddNewItem();
        break;
    case ToolbarButtons::Remove:
        m_pListBinder->RemoveSelectedItem();
        break;
    case ToolbarButtons::Edit:
        m_pListBinder->OpenItemEditor();
        break;
    case ToolbarButtons::MoveUp:
        m_pListBinder->MoveItemUp();
        break;
    case ToolbarButtons::MoveDown:
        m_pListBinder->MoveItemDown();
        break;
    case ToolbarButtons::SortAsc:
        m_pListBinder->SortItems(SortDirection::Ascending);
        break;
    case ToolbarButtons::SortDesc:
        m_pListBinder->SortItems(SortDirection::Descending);
        break;
    default:
        break;
    }
}
