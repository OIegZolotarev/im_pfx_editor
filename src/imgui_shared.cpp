#include "opengl_shared.h"
#include "imgui_shared.h"

glm::vec2 ToGLMVec2(ImVec2 pt)
{
    return glm::vec2(pt.x, pt.y);
}

glm::vec4 ToGLMVec4(ImVec4 pt)
{
    return glm::vec4(pt.x, pt.y, pt.z, pt.w);
}

ImVec2 FromGLMVec2(glm::vec2 pt)
{
    return ImVec2(pt.x, pt.y);
}

ImVec4 FromGLMVec4(glm::vec4 pt)
{
    return ImVec4(pt.x, pt.y, pt.z, pt.w);
}