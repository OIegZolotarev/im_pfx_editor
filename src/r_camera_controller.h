/*
    LightBaker3000 Frontend project,
    (c) 2022-2025 CrazyRussian
*/

#pragma once


#include <cstddef>
#include <functional>
#include <stack>   
  
#include "common.h"
#include "event_handler.h"
#include "mathlib.h"
#include "variant.h" 
  
#include "viewport.h"

enum class CameraMouseModes
{
    Pan,
    Rotate, 
    Zoom,
    None
};  
  
#define PITCH 0 
#define YAW   1
#define ROLL  2 

typedef std::function<void(bool bHit, const SDL_Event &event)> pfnKeyStrokeCallback;

enum class CameraControlScheme
{
    ValveHammerEditor = 0,
    BlenderTouchpad,
    Blender
};

class CameraCommandKeyStroke
{
    int m_iDebugTag = -1; 

    int m_KeyboardKeys[9];
    int m_MouseKeys = 0;
    int m_WheelConditions = 0;
    bool m_bNeedOtherKeys = false;

    glm::vec3 m_vecModeSpeed = {0,0,0};

    pfnKeyStrokeCallback m_Callback;

  public:
    void SetDebugTag(int tag)
    {
        m_iDebugTag = tag;
    };
    int GetDebugTag()
    {
        return m_iDebugTag;
    }

    CameraCommandKeyStroke();
    ~CameraCommandKeyStroke();

    void AddKeyboardKey(const char *keyName);
    void AddMouseKey(int keyMask);

    bool AreKeysPressed();

    void SetCallback(pfnKeyStrokeCallback callback);
    void ExecuteCallback(bool bHit, const SDL_Event &event);
    void SetWheelDirection(int direction, bool needOtherKeys);

    bool CanBeExecuteFromMouseWheel(int direction);
};

#include <vector>
typedef std::vector<CameraCommandKeyStroke *> KeystrokeList;

class CameraController : public IEventHandler
{
    CameraMouseModes m_Mode = CameraMouseModes::None;

    KeystrokeList m_vKeyStrokesBlenderTouchpad;
    KeystrokeList m_vKeyStrokesBlender;
    KeystrokeList m_vKeyStrokesVHE;

    void SetupKeystrokes();

    void SetupCommonKeystrokesCallbacks();
    void SetupKeystrokesBlenderTouchpad();
    void SetupKeystrokesBlender();
    void SetupKeystrokesVHE();

    pfnKeyStrokeCallback callbackRotate;
    pfnKeyStrokeCallback callbackPan;
    pfnKeyStrokeCallback callbackZoom;
    pfnKeyStrokeCallback callbackZoomIn;
    pfnKeyStrokeCallback callbackZoomOut;

    pfnKeyStrokeCallback callbackToggleFPSNavigation;
    pfnKeyStrokeCallback callbackMoveForward;
    pfnKeyStrokeCallback callbackMoveBackward;
    pfnKeyStrokeCallback callbackStrafeLeft;
    pfnKeyStrokeCallback callbackStrafeRight;
    pfnKeyStrokeCallback callbackMoveup;

    void SetForwardSpeed(const int moveSpeed);
    void SetStrafeSpeed(const int moveSpeed);
    void SetUpSpeed(const int moveSpeed);

    bool m_bFPSNavigation = false;

    VariantValue *m_pFov   = nullptr;
    VariantValue *m_pZNear = nullptr;
    VariantValue *m_pZFar  = nullptr;

    VariantValue *m_pAccelSpeed           = nullptr;
    VariantValue *m_pDeccelSpeed          = nullptr;
    VariantValue *m_pMoveSpeed            = nullptr;
    VariantValue *m_pCameraControlsScheme = nullptr;

    VariantValue *m_pCameraSensivityRotation = nullptr;
    VariantValue *m_pCameraSensivityPan = nullptr;
    VariantValue *m_pCameraSensivityZoom = nullptr;

    glm::vec3 m_vForward = {1.f, 0, 0};
    glm::vec3 m_vRight   = {0, 1, 0};
    glm::vec3 m_vUp      = {0, 0, 1};
    glm::vec3 m_Origin   = {0, 0, 0};
    glm::vec3 m_Angles   = {90, 0, 0};

    // Transition animation
    bool m_bInTransition = false;
    float m_flTransitionProgress = 0.0f;
    
    glm::vec3 m_TranstionStartingAngles               = {90, 0, 0};
    glm::vec3 m_TranstionoEndingAngles               = {90, 0, 0};

    glm::mat4 m_matProjection;
    glm::mat4 m_matModelView;

    std::stack<glm::mat4> m_matModelViewStack;
    std::stack<glm::mat4> m_matProjectionStack;

    // TODO: port later
    // Frustum m_Frustum;

    glm::vec3 m_IdealMoveSpeeds   = {0.f,0.f,0.f};
    glm::vec3 m_CurrentMoveSpeeds = {0.f,0.f,0.f};

    void SetupPerspectiveMatrix();
    void SetupModelViewMatrix();

    int MouseMotionEvent(const SDL_Event &event, const float flFrameDelta);
    int ButtonEvent(const SDL_Event &event);
    int MouseWheelEvent(const SDL_Event &event, const float flFrameDelta);

    void DoZoomIn();
    void DoZoomOut();

    void UpdateOrientation(float flFrameDelta);
    bool CalcMovementSpeeds(float flFrameDelta); 

    class Viewport *m_pViewport;

  public:

    CameraController(Viewport * pViewport);
    ~CameraController();

    const glm::vec3 &GetAngles() const;
    const glm::vec3 &GetPosition() const;

    void SetOrigin(const glm::vec3 newPos);
    void SetAngles(const glm::vec3 newAngles);

    void Apply(float flFrameDelta);
    void Reset();

    void PopMatrices(int which, bool bApply);
    void PushMatrices(int which);

    const glm::mat4 GetViewMatrix() const;
    const glm::mat4 GetProjectionMatrix() const;

    float* GetViewMatrixPtr() const;
    float* GetProjectionMatrixPtr() const;

    int HandleEvent(bool bWasHandled, const SDL_Event &e, const float flFrameDelta) override;

    glm::vec3 GetRightVector();
    glm::vec3 GetUpVector();
    glm::vec3 GetForwardVector();

    void LookAtPoint(glm::vec3 pos);

    void FormatControlsTip(std::string & dst);
    const float GetZFar();
    const float GetFOVY(const float aspect);
    const float AspectRatio();
    const float GetZNear();
    const float getFOVX();

    // TODO: port later
    //Frustum *GetFrustum();
    glm::vec3 GetMoveSpeed();
    bool      IsFPSNavigationEngaged();
    void      CopyOrientation(CameraController *param1);

    const glm::vec3 ScreenToWorld(const glm::vec3 normalizedDeviceCoords) const;

    void ExecuteTransition(glm::vec3 targetAngles);
};