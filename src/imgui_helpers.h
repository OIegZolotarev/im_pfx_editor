/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/

#pragma once

#include "common.h"
#include "common_resources.h"
#include "common_icons.h"



namespace ImGuiHelpers
{
void Init();

bool ImageButtonWithText(ImTextureID texId, const char *label, const ImVec2 &imageSize, const ImVec2 &uv0,
                         const ImVec2 &uv1, int frame_padding, const ImVec4 &bg_col, const ImVec4 &tint_col);

bool ButtonWithCommonTextureIcon(CommonTextures icon, const char *label, const float icon_size, ImVec4 tint = {1, 1, 1, 1});

bool ButtonWithCommonIcon(CommonIcons icon, const char *label, const float icon_size, ImVec4 tint = {1, 1, 1, 1});

void DisplayCommonIcon(CommonIcons icon, const float size);


} // namespace ImGuiHelpers
