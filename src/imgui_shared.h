#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "mathlib.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>
#include <misc/cpp/imgui_stdlib.h>

// #include <ImGuiFileDialog/ImGuiFileDialog.h>
// #include <ImGuizmo/ImGuizmo.h>

glm::vec2 ToGLMVec2(ImVec2 pt);
glm::vec4 ToGLMVec4(ImVec4 pt);

ImVec2 FromGLMVec2(glm::vec2 pt);
ImVec4 FromGLMVec4(glm::vec4 pt);