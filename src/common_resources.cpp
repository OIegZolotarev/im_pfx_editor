/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/

#include "common.h"
#include "application.h"
#include "common_resources.h"



std::unordered_map<CommonTextures, GLTexture *> *g_CommonIcons = nullptr;

void LoadIcon(CommonTextures id, const char *filePath)
{
    //FileData *pData = Application::GetFileSystem()->LoadFile(filePath);

    TextureManager::Instance();
    std::pair<CommonTextures, GLTexture *> p(id, TextureManager::LoadTextureSynch(filePath));
    g_CommonIcons->insert(p);
}

void DestroyCommonResources()
{
    g_CommonIcons->clear();
}

void InitCommonResources()
{
    g_CommonIcons = new std::unordered_map<CommonTextures, GLTexture *>;

    // clang-format off

	LoadIcon(CommonTextures::LoadFile        , "res/ui/icons/loadfile.png");
	LoadIcon(CommonTextures::Bake            , "res/ui/icons/bake.png");
	LoadIcon(CommonTextures::DirectLight     , "res/ui/icons/directlight.png");
	LoadIcon(CommonTextures::OmniLight       , "res/ui/icons/omnilight.png");
	LoadIcon(CommonTextures::SpotLight       , "res/ui/icons/spotlight.png");
	LoadIcon(CommonTextures::ListAdd         , "res/ui/common_icons/lists/add.png");
    LoadIcon(CommonTextures::ListRemove      , "res/ui/common_icons/lists/remove.png");
    LoadIcon(CommonTextures::ListEdit        , "res/ui/common_icons/lists/edit.png");
    LoadIcon(CommonTextures::ListMoveUp      , "res/ui/common_icons/lists/move_up.png");
    LoadIcon(CommonTextures::ListMoveDown    , "res/ui/common_icons/lists/move_down.png");
    LoadIcon(CommonTextures::ListSortDesc    , "res/ui/common_icons/lists/sort_desc.png");
    LoadIcon(CommonTextures::ListSortAsc     , "res/ui/common_icons/lists/sort_asc.png");    
    LoadIcon(CommonTextures::InputFieldClear , "res/ui/common_icons/input_field/clear.png");
    LoadIcon(CommonTextures::InputFieldMore  , "res/ui/common_icons/input_field/more.png");    
    
    LoadIcon(CommonTextures::White           , "res/textures/default/white.png");
    LoadIcon(CommonTextures::Emo             , "res/textures/default/emo.png");

    // clang-format on
}

GLTexture *GetCommonIcon(CommonTextures icon_id)
{
    if (!g_CommonIcons->count(icon_id))
        return 0;

    return (*g_CommonIcons)[icon_id];
}
