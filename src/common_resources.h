/*
	LightBaker3000 Frontend project,
	(c) 2022 CrazyRussian
*/

#pragma once
#include "gl_texture.h"

enum class CommonTextures
{
	LoadFile = 0,
	SpotLight,
	DirectLight,
	OmniLight,
	Bake,
	ListAdd,
	ListRemove,
	ListEdit,
	ListMoveUp,
	ListMoveDown,	
	ListSortAsc,
    ListSortDesc,
    InputFieldMore,
    InputFieldClear,
	White,
	Emo
};

void			InitCommonResources();
GLTexture*	GetCommonIcon(CommonTextures icon_id);