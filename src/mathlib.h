#pragma once

#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/compatibility.hpp>
//#include <glm/gtc/quaternion.hpp>
// #include <glm/gtx/euler_angles.hpp>
//#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>

inline float AngleMod(float val)
{
    if (val > 360)
        return val - 360;
    else if (val < 0)
        return val + 360;

    return val;
}