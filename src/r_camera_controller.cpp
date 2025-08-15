/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/

#include "application.h"
#include "common.h"
#include "opengl_shared.h"

#include "persistent.h"

#include "r_camera_controller.h"


//#define OLD_GL

CameraController::CameraController(Viewport * pViewport)
{
    auto settings = Application::Instance()->GetPersistentStorage();

    m_pFov                     = settings->GetSetting(ApplicationSettings::CameraFov);
    m_pZNear                   = settings->GetSetting(ApplicationSettings::CameraZNear);
    m_pZFar                    = settings->GetSetting(ApplicationSettings::CameraZFar);
    m_pAccelSpeed              = settings->GetSetting(ApplicationSettings::CameraAccel);
    m_pDeccelSpeed             = settings->GetSetting(ApplicationSettings::CameraDecel);
    m_pMoveSpeed               = settings->GetSetting(ApplicationSettings::CameraMovementSpeed);
    m_pCameraControlsScheme    = settings->GetSetting(ApplicationSettings::CameraControlScheme);
    m_pCameraSensivityRotation = settings->GetSetting(ApplicationSettings::CameraMouseSensivityRotating);
    m_pCameraSensivityPan      = settings->GetSetting(ApplicationSettings::CameraMouseSensivityPaning);
    m_pCameraSensivityZoom     = settings->GetSetting(ApplicationSettings::CameraMouseSensivityZooming);

    m_Origin = {0, 0, 0};
    m_Angles = {0, 0, 0};

    m_matModelView  = glm::mat4(1.f);
    m_matProjection = glm::mat4(1.f);

    SetupCommonKeystrokesCallbacks();
    SetupKeystrokes();    

    m_pViewport = pViewport;
}

CameraController::~CameraController()
{
    ClearPointersVector(m_vKeyStrokesBlenderTouchpad);
    ClearPointersVector(m_vKeyStrokesBlender);
    ClearPointersVector(m_vKeyStrokesVHE);
}

void CameraController::SetupKeystrokes()
{
    SetupKeystrokesBlenderTouchpad();
    SetupKeystrokesBlender();
    SetupKeystrokesVHE();
}

void CameraController::SetupKeystrokesBlenderTouchpad()
{
    CameraCommandKeyStroke *keystrokeRotate = new CameraCommandKeyStroke();
    keystrokeRotate->AddMouseKey(SDL_BUTTON_LMASK);
    keystrokeRotate->AddKeyboardKey("Left Alt");
    keystrokeRotate->SetCallback(callbackRotate);

    m_vKeyStrokesBlenderTouchpad.push_back(keystrokeRotate);

    // Panning

    CameraCommandKeyStroke *keystrokePan = new CameraCommandKeyStroke();
    keystrokePan->AddMouseKey(SDL_BUTTON_LMASK);
    keystrokePan->AddKeyboardKey("Left Shift");
    keystrokePan->AddKeyboardKey("Left Alt");
    keystrokePan->SetCallback(callbackPan);

    m_vKeyStrokesBlenderTouchpad.push_back(keystrokePan);

    // Zoom

    CameraCommandKeyStroke *keystrokeZoom = new CameraCommandKeyStroke();
    keystrokeZoom->AddMouseKey(SDL_BUTTON_LMASK);
    keystrokeZoom->AddKeyboardKey("Left Alt");
    keystrokeZoom->AddKeyboardKey("Left Ctrl");
    keystrokeZoom->SetCallback(callbackZoom);
    m_vKeyStrokesBlenderTouchpad.push_back(keystrokeZoom);

    // Wheel and +-

    CameraCommandKeyStroke *keystrokeZoomIn = new CameraCommandKeyStroke();
    keystrokeZoomIn->AddKeyboardKey("Keypad +");
    keystrokeZoomIn->SetWheelDirection(1, false);
    keystrokeZoomIn->SetCallback(callbackZoomIn);
    m_vKeyStrokesBlenderTouchpad.push_back(keystrokeZoomIn);

    CameraCommandKeyStroke *keystrokeZoomOut = new CameraCommandKeyStroke();
    keystrokeZoomOut->AddKeyboardKey("Keypad -");
    keystrokeZoomOut->SetWheelDirection(-1, false);
    keystrokeZoomOut->SetCallback(callbackZoomOut);
    m_vKeyStrokesBlenderTouchpad.push_back(keystrokeZoomOut);
}

void CameraController::SetupKeystrokesBlender()
{
    // Rotate

    CameraCommandKeyStroke *keystrokeRotate = new CameraCommandKeyStroke();
    keystrokeRotate->AddMouseKey(SDL_BUTTON_MMASK);
    keystrokeRotate->SetCallback(callbackRotate);

    m_vKeyStrokesBlender.push_back(keystrokeRotate);

    // Panning

    CameraCommandKeyStroke *keystrokePan = new CameraCommandKeyStroke();
    keystrokePan->AddMouseKey(SDL_BUTTON_MMASK);
    keystrokePan->AddKeyboardKey("Left Shift");
    keystrokePan->SetCallback(callbackPan);

    m_vKeyStrokesBlender.push_back(keystrokePan);

    // Zoom

    CameraCommandKeyStroke *keystrokeZoom = new CameraCommandKeyStroke();
    keystrokeZoom->AddMouseKey(SDL_BUTTON_MMASK);
    keystrokeZoom->AddKeyboardKey("Left Ctrl");
    keystrokeZoom->SetCallback(callbackZoom);
    keystrokeZoom->SetDebugTag(42);
    m_vKeyStrokesBlender.push_back(keystrokeZoom);

    // Zoom mouse

    CameraCommandKeyStroke *keystrokeZoomIn = new CameraCommandKeyStroke();
    keystrokeZoomIn->AddKeyboardKey("Keypad +");
    keystrokeZoomIn->SetWheelDirection(1, false);
    keystrokeZoomIn->SetCallback(callbackZoomIn);
    m_vKeyStrokesBlender.push_back(keystrokeZoomIn);

    CameraCommandKeyStroke *keystrokeZoomOut = new CameraCommandKeyStroke();
    keystrokeZoomOut->AddKeyboardKey("Keypad -");
    keystrokeZoomOut->SetWheelDirection(-1, false);
    keystrokeZoomOut->SetCallback(callbackZoomOut);
    m_vKeyStrokesBlender.push_back(keystrokeZoomOut);
}

void CameraController::SetupCommonKeystrokesCallbacks()
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value" // Make compiler happy...

#define DEBUG_KEYSTROKES printf

    callbackRotate = pfnKeyStrokeCallback([&](bool bHit, const SDL_Event &event) -> void {
        DEBUG_KEYSTROKES("callbackRotate(%d)\n", bHit);

        if (bHit)
            m_Mode = CameraMouseModes::Rotate;
        else
            m_Mode = CameraMouseModes::None;
    });

    callbackPan = pfnKeyStrokeCallback([&](bool bHit, const SDL_Event &event) -> void {
        DEBUG_KEYSTROKES("callbackPan(%d) event == %d\n", bHit, event.type);

        if (bHit)
            m_Mode = CameraMouseModes::Pan;
        else
            m_Mode = CameraMouseModes::None;
    });

    callbackZoom = pfnKeyStrokeCallback([&](bool bHit, const SDL_Event &event) -> void {
        DEBUG_KEYSTROKES("callbackZoom(%d)\n", bHit);

        if (bHit)
            m_Mode = CameraMouseModes::Zoom;
        else
            m_Mode = CameraMouseModes::None;
    });

    callbackZoomIn = pfnKeyStrokeCallback([&](bool bHit, const SDL_Event &event) -> void {
        DEBUG_KEYSTROKES("callbackZoomIn(%d)\n", bHit);

        if (bHit)
            DoZoomIn();
        //else
        //    m_Mode = CameraMouseModes::None;
    });

    callbackZoomOut = pfnKeyStrokeCallback([&](bool bHit, const SDL_Event &event) -> void {
        DEBUG_KEYSTROKES("callbackZoomOut(%d)\n", bHit);

        if (bHit)
            DoZoomOut();
        //else
        //    m_Mode = CameraMouseModes::None;
    });

    callbackToggleFPSNavigation = pfnKeyStrokeCallback([&](bool bHit, const SDL_Event &event) -> void {
        if (bHit)        
            Con_Printf("callbackToggleFPSNavigation(hit=%d) (%s)\n", bHit, m_pViewport->Name());

        if (bHit)
        {
            auto windowHandle = Application::GetMainWindow()->Handle();
            // auto viewport     = Application::GetMainWindow()->Get3DGLViewport();

            int winWidth  = Application::GetMainWindow()->Width();
            int winHeight = Application::GetMainWindow()->Height();

            SDL_RaiseWindow(windowHandle);
            SDL_WarpMouseInWindow(windowHandle, winWidth / 2, winHeight / 2);

            m_bFPSNavigation = !m_bFPSNavigation;

            // Con_Printf("FPS state %d in %s\n", m_bFPSNavigation, m_pViewport->Name());

            if (m_bFPSNavigation)
            {
                //Application::Instance()->SetupEventsRedirection(true, m_pViewport->GetPlatformWindow());
                Application::Instance()->HideMouseCursor();
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
            else
            {
                SDL_SetRelativeMouseMode(SDL_FALSE);
                Application::Instance()->ShowMouseCursor();
                //Application::Instance()->SetupEventsRedirection(false, m_pViewport->GetPlatformWindow());

                m_Mode = CameraMouseModes::None;
            }
        }
    });

    callbackMoveForward = pfnKeyStrokeCallback([&](bool bHit, const SDL_Event &event) -> void {
        if (bHit)
            SetForwardSpeed(m_pMoveSpeed->GetFloat());
        else
            SetForwardSpeed(0);
    });

    callbackMoveBackward = pfnKeyStrokeCallback([&](bool bHit, const SDL_Event &event) -> void {
        if (bHit)
            SetForwardSpeed(-m_pMoveSpeed->GetFloat());
        else
            SetForwardSpeed(0);
    });

    callbackStrafeLeft = pfnKeyStrokeCallback([&](bool bHit, const SDL_Event &event) -> void {
        if (bHit)
            SetStrafeSpeed(-m_pMoveSpeed->GetFloat());
        else
            SetStrafeSpeed(0);
    });

    callbackStrafeRight = pfnKeyStrokeCallback([&](bool bHit, const SDL_Event &event) -> void {
        if (bHit)
            SetStrafeSpeed(m_pMoveSpeed->GetFloat());
        else
            SetStrafeSpeed(0);
    });

    callbackMoveup = pfnKeyStrokeCallback([&](bool bHit, const SDL_Event &event) -> void {
        if (bHit)
            SetUpSpeed(m_pMoveSpeed->GetFloat());
        else
            SetUpSpeed(0);
    });

    #pragma clang diagnostic pop    
}

const glm::vec3 &CameraController::GetAngles() const
{
    return m_Angles;
}

const glm::vec3 &CameraController::GetPosition() const
{
    return m_Origin;
}

void CameraController::SetOrigin(const glm::vec3 newPos)
{
    m_Origin = newPos;
}

void CameraController::SetAngles(const glm::vec3 newAngles)
{
    m_Angles = newAngles;
}

void CameraController::Apply(float flFrameDelta)
{
    UpdateOrientation(flFrameDelta);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    SetupPerspectiveMatrix();

    glDepthRange(0, 1);

#ifdef OLD_GL
    glMatrixMode(GL_MODELVIEW);
#endif

    SetupModelViewMatrix();

    // m_Frustum.InitPerspective(this);
}

void CameraController::Reset()
{
}

void CameraController::PopMatrices(int which, bool bApply)
{
}

void CameraController::PushMatrices(int which)
{
}

const glm::mat4 CameraController::GetViewMatrix() const
{
    return m_matModelView;
}

const glm::mat4 CameraController::GetProjectionMatrix() const
{
    return m_matProjection;
}

float *CameraController::GetViewMatrixPtr() const
{
    return (float *)&m_matModelView[0][0];
}

float *CameraController::GetProjectionMatrixPtr() const
{
    return (float *)&m_matProjection[0][0];
}

int CameraController::HandleEvent(bool bWasHandled, const SDL_Event &e, const float flFrameDelta)
{
    switch (e.type)
    {
    case SDL_MOUSEMOTION:
        return MouseMotionEvent(e, flFrameDelta);
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    case SDL_KEYUP:
    case SDL_KEYDOWN:
        return ButtonEvent(e);
        break;
    case SDL_MOUSEWHEEL:
        return MouseWheelEvent(e, flFrameDelta);
        break;
    }

    return 0;
}

void CameraController::UpdateOrientation(float flFrameDelta)
{
    if (m_bInTransition)
    {
        m_flTransitionProgress += flFrameDelta * 2;

        if (m_flTransitionProgress >= 1.f)
        {
            m_Angles        = m_TranstionoEndingAngles;
            m_bInTransition = false;
            return;
        }

        m_Angles = glm::lerp(m_TranstionStartingAngles, m_TranstionoEndingAngles, m_flTransitionProgress);
        m_pViewport->FlagUpdate();
        return;
    }

    bool recalcPosition = CalcMovementSpeeds(flFrameDelta);

    if (m_bFPSNavigation)
    {
        Application::Instance()->HideMouseCursor();
        SDL_SetRelativeMouseMode(SDL_TRUE);

        int x, y;
        SDL_GetMouseState(&x, &y);

        m_Mode = CameraMouseModes::Rotate;

        m_Angles[PITCH] = std::clamp(m_Angles[PITCH], -90.f, 90.f);

        // SDL_WarpMouseInWindow(Application::GetMainWindow()->Handle(), winWidth / 2, winHeight / 2);

        glm::vec3 old = m_Origin;
        m_Origin += (m_CurrentMoveSpeeds[0] * flFrameDelta) * m_vForward +
                    (m_CurrentMoveSpeeds[1] * flFrameDelta) * m_vRight +
                    (m_CurrentMoveSpeeds[2] * flFrameDelta) * m_vUp;

        if (glm::length2(old - m_Origin) > 1.f)
            recalcPosition = true;
    }
    else
    {
        if (recalcPosition)
            m_Origin += (m_CurrentMoveSpeeds[0] * flFrameDelta) * m_vForward +
                        (m_CurrentMoveSpeeds[1] * flFrameDelta) * m_vRight +
                        (m_CurrentMoveSpeeds[2] * flFrameDelta) * m_vUp;
    }

    if (recalcPosition)
    {
        m_pViewport->FlagUpdate();
        // Application::GetMainWindow()->UpdateStatusbar(1 << StatusbarField::Position);
    }
}

bool CameraController::CalcMovementSpeeds(float flFrameDelta)
{
    if (!m_bFPSNavigation)
        for (int i = 0; i < 3; i++)
            m_IdealMoveSpeeds[i] = 0;

    float acceleration = m_pAccelSpeed->GetFloat();
    float deceleration = m_pDeccelSpeed->GetFloat();

    bool recalcPosition = false;

    for (int i = 0; i < 3; i++)
    {
        if (m_IdealMoveSpeeds[i] > 1.f)
        {
            m_CurrentMoveSpeeds[i] += acceleration * flFrameDelta;
            m_CurrentMoveSpeeds[i] = std::min(m_IdealMoveSpeeds[i], m_CurrentMoveSpeeds[i]);

            recalcPosition = true;
        }
        else if (m_IdealMoveSpeeds[i] < -1.f)
        {
            m_CurrentMoveSpeeds[i] -= acceleration * flFrameDelta;
            m_CurrentMoveSpeeds[i] = std::max(m_IdealMoveSpeeds[i], m_CurrentMoveSpeeds[i]);

            recalcPosition = true;
        }
        else if (abs(m_CurrentMoveSpeeds[i]) > 1.f)
        {
            if (m_CurrentMoveSpeeds[i] > 0)
            {
                m_CurrentMoveSpeeds[i] -= deceleration * flFrameDelta;
                m_CurrentMoveSpeeds[i] = std::max(m_CurrentMoveSpeeds[i], 0.f);
            }
            else
            {
                m_CurrentMoveSpeeds[i] += deceleration * flFrameDelta;
                m_CurrentMoveSpeeds[i] = std::min(m_CurrentMoveSpeeds[i], 0.f);
            }

            recalcPosition = true;
        }
        else
            m_CurrentMoveSpeeds[i] = 0;
    }

    return recalcPosition;
}

void CameraController::LookAtPoint(glm::vec3 pos)
{
    m_Origin = pos - m_vForward * 30.f;
}

void CameraController::FormatControlsTip(std::string &dst)
{
    CameraControlScheme scheme = (CameraControlScheme)m_pCameraControlsScheme->GetEnumValue();

    switch (scheme)
    {
    case CameraControlScheme::ValveHammerEditor:
        dst = "Using VHE controls: Z - toggle 3d navigation, W/A/S/D and Space to move";
        break;
    case CameraControlScheme::BlenderTouchpad:
        dst = ("Using Blender (touchpad) controls: Mouse left button + Left Alt - to rotate, Mouse left button + Left "
               "Alt + Left Shift - to pan, Mouse left button + Left Alt + Left Ctrl - to zoom");
        break;
    case CameraControlScheme::Blender:
        dst = ("Using Blender controls: Middle mouse - to rotate, Middle mouse + Left Shift - to pan, Middle mouse + "
               "Left Ctrl - to zoom");
        break;
    default:
        break;
    }
}

const float CameraController::GetZFar()
{
    return m_pZFar->GetFloat();
}

const float CameraController::GetFOVY(const float aspect)
{
    double fov = glm::radians(m_pFov->GetFloat());
    return glm::degrees(2 * atan(tan(fov / 2) / aspect)); // Ìîÿ ôîðìóëà - ïëàâàåò?
}

const float CameraController::AspectRatio()
{
    glm::vec2 vp = m_pViewport->GetClientArea();

    float m_iWidth  = vp[0];
    float m_iHeight = vp[1];

    if (!m_iWidth || !m_iHeight)
        return 1;

    double aspect = m_iWidth / m_iHeight;
    return aspect;
}

const float CameraController::GetZNear()
{
    return m_pZNear->GetFloat();
}

const float CameraController::getFOVX()
{
    return m_pFov->GetFloat();
}

// Frustum *CameraController::GetFrustum()
// {
//     return &m_Frustum;
// }

glm::vec3 CameraController::GetMoveSpeed()
{
    return (m_IdealMoveSpeeds);
}

bool CameraController::IsFPSNavigationEngaged()
{
    return m_bFPSNavigation;
}

void CameraController::CopyOrientation(CameraController *param1)
{
    SetOrigin(param1->m_Origin);
    SetAngles(param1->m_Angles);
}

const glm::vec3 CameraController::ScreenToWorld(const glm::vec3 normalizedDeviceCoords) const
{
// Projection matrix seems fine...
#if 0
    double fov = glm::radians(m_pFov->GetFloat());

    glm::vec2 vp = m_pViewport->GetClientArea();

    float m_iWidth  = vp[0];
    float m_iHeight = vp[1];

    double aspect = m_iWidth / m_iHeight;
    double fov_y  = 2 * atan(tan(fov / 2) / aspect); // Ìîÿ ôîðìóëà - ïëàâàåò?
    double dbg    = glm::degrees(fov_y);

    glm::mat4 proj = glm::perspective(fov_y, aspect, (double)m_pZNear->GetFloat(), (double)m_pZFar->GetFloat());

#endif

    glm::mat4 unprojectionMatrix = glm::inverse(m_matProjection * m_matModelView);
    glm::vec4 result             = unprojectionMatrix * glm::vec4(normalizedDeviceCoords, 1);

    result *= 1 / result.w;

    glm::vec4 test = (m_matProjection * (m_matModelView * result));

    float error = glm::length(normalizedDeviceCoords - test.xyz());

    return glm::vec3(result.xyz());
}

void CameraController::ExecuteTransition(glm::vec3 targetAngles)
{
    m_TranstionStartingAngles = m_Angles;
    m_TranstionoEndingAngles  = targetAngles;

    m_bInTransition        = true;
    m_flTransitionProgress = 0;
}

glm::vec3 CameraController::GetRightVector()
{
    return m_vRight;
}

glm::vec3 CameraController::GetUpVector()
{
    return m_vUp;
}

glm::vec3 CameraController::GetForwardVector()
{
    return m_vForward;
}

void CameraController::SetupKeystrokesVHE()
{
    CameraCommandKeyStroke *keystrokeMoveForward = new CameraCommandKeyStroke();
    keystrokeMoveForward->AddKeyboardKey("W");
    keystrokeMoveForward->SetCallback(callbackMoveForward);
    m_vKeyStrokesVHE.push_back(keystrokeMoveForward);

    CameraCommandKeyStroke *keystrokeMoveBackward = new CameraCommandKeyStroke();
    keystrokeMoveBackward->AddKeyboardKey("S");
    keystrokeMoveBackward->SetCallback(callbackMoveBackward);
    m_vKeyStrokesVHE.push_back(keystrokeMoveBackward);

    CameraCommandKeyStroke *keystrokeStrafeRight = new CameraCommandKeyStroke();
    keystrokeStrafeRight->AddKeyboardKey("D");
    keystrokeStrafeRight->SetCallback(callbackStrafeRight);
    m_vKeyStrokesVHE.push_back(keystrokeStrafeRight);

    CameraCommandKeyStroke *keystrokeStrafeLeft = new CameraCommandKeyStroke();
    keystrokeStrafeLeft->AddKeyboardKey("A");
    keystrokeStrafeLeft->SetCallback(callbackStrafeLeft);
    m_vKeyStrokesVHE.push_back(keystrokeStrafeLeft);

    CameraCommandKeyStroke *keystrokeMoveup = new CameraCommandKeyStroke();
    keystrokeMoveup->AddKeyboardKey("Space");
    keystrokeMoveup->SetCallback(callbackMoveup);
    m_vKeyStrokesVHE.push_back(keystrokeMoveup);

    CameraCommandKeyStroke *keystrokeToggleFPSNavigation = new CameraCommandKeyStroke();
    keystrokeToggleFPSNavigation->AddKeyboardKey("Z");
    keystrokeToggleFPSNavigation->SetCallback(callbackToggleFPSNavigation);
    m_vKeyStrokesVHE.push_back(keystrokeToggleFPSNavigation);

    // Wheel and +-

    CameraCommandKeyStroke *keystrokeZoomIn = new CameraCommandKeyStroke();
    keystrokeZoomIn->AddKeyboardKey("Keypad +");
    keystrokeZoomIn->SetWheelDirection(1, false);
    keystrokeZoomIn->SetCallback(callbackZoomIn);
    m_vKeyStrokesVHE.push_back(keystrokeZoomIn);

    CameraCommandKeyStroke *keystrokeZoomOut = new CameraCommandKeyStroke();
    keystrokeZoomOut->AddKeyboardKey("Keypad -");
    keystrokeZoomOut->SetWheelDirection(-1, false);
    keystrokeZoomOut->SetCallback(callbackZoomOut);
    m_vKeyStrokesVHE.push_back(keystrokeZoomOut);
}

void CameraController::SetForwardSpeed(const int moveSpeed)
{
    m_IdealMoveSpeeds[0] = moveSpeed;
}

void CameraController::SetStrafeSpeed(const int moveSpeed)
{
    m_IdealMoveSpeeds[1] = moveSpeed;
}

void CameraController::SetUpSpeed(const int moveSpeed)
{
    m_IdealMoveSpeeds[2] = moveSpeed;
}

int CameraController::MouseMotionEvent(const SDL_Event &_event, const float flFrameDelta)
{
    auto  event  = _event.motion;
    float flDist = m_pMoveSpeed->GetFloat();

    // Con_Printf("MouseMotion: %d %d [%d]\n", event.xrel, event.yrel, m_Mode);

    if (m_bFPSNavigation || m_Mode != CameraMouseModes::None)
    {
        m_pViewport->FlagUpdate();
    }

    switch (m_Mode)
    {
    case CameraMouseModes::Pan: {
        float xDelta = event.xrel * flFrameDelta * flDist * m_pCameraSensivityPan->GetFloat();
        float yDelta = event.yrel * flFrameDelta * flDist * m_pCameraSensivityPan->GetFloat();

        m_Origin += m_vUp * yDelta + m_vRight * -xDelta;
        return EVENT_FINISHED;
    }
    break;

    case CameraMouseModes::Rotate: {
        float xDelta = event.xrel;
        float yDelta = event.yrel;

        m_Angles[PITCH] += yDelta * m_pCameraSensivityRotation->GetFloat();
        m_Angles[YAW] -= xDelta * m_pCameraSensivityRotation->GetFloat();

        m_Angles[YAW]   = AngleMod(m_Angles[YAW]);
        m_Angles[PITCH] = std::clamp(m_Angles[PITCH], -90.f, 90.f);

        return EVENT_FINISHED;
    }
    break;
    case CameraMouseModes::Zoom: {
        float xDelta = event.xrel * flFrameDelta * 100 * m_pCameraSensivityZoom->GetFloat();
        float yDelta = event.yrel * flFrameDelta * 100 * m_pCameraSensivityZoom->GetFloat();

        m_Origin += m_vForward * (xDelta + yDelta);
        return EVENT_FINISHED;
    }

    case CameraMouseModes::None:
        break;
    default:
        break;
    }

    return 0;
}

int CameraController::ButtonEvent(const SDL_Event &_event)
{
    auto event = _event.button;

    KeystrokeList *keystrokeSet = nullptr;

    CameraControlScheme controlScheme = (CameraControlScheme)m_pCameraControlsScheme->GetEnumValue();
    switch (controlScheme)
    {
    case CameraControlScheme::ValveHammerEditor:
        keystrokeSet = &m_vKeyStrokesVHE;
        break;
    case CameraControlScheme::BlenderTouchpad:
        keystrokeSet = &m_vKeyStrokesBlenderTouchpad;
        break;
    case CameraControlScheme::Blender:
        keystrokeSet = &m_vKeyStrokesBlender;
        break;
    default:
        break;
    }

    if (!keystrokeSet)
        return 0;

    CameraCommandKeyStroke *best = 0;

    for (auto &it : *keystrokeSet)
    {
        if (it->AreKeysPressed())
        {
            best = it;
        }
        else
            it->ExecuteCallback(false, _event);
    }

    if (best)
    {
        best->ExecuteCallback(true, _event);
        return EVENT_FINISHED;
    }
    else
        return 0;
}

int CameraController::MouseWheelEvent(const SDL_Event &_event, const float flFrameDelta)
{
    auto  event  = _event.motion;
    float flDist = glm::length(m_Origin);

    if (isnan(flDist))
    {
        flDist   = 10;
        m_Origin = glm::vec3(0, 0, 10);
        return 0;
    }

    switch (m_Mode)
    {
    case CameraMouseModes::Pan: {
        float xDelta = event.xrel * flFrameDelta * flDist;
        float yDelta = event.yrel * flFrameDelta * flDist;

        m_Origin += m_vUp * yDelta + m_vRight * -xDelta;

        return EVENT_FINISHED;
    }
    break;
    case CameraMouseModes::Rotate: {
        float xDelta = event.xrel * flFrameDelta * 20;
        float yDelta = event.yrel * flFrameDelta * 20;

        m_Angles[PITCH] += yDelta;
        m_Angles[YAW] -= xDelta;

        return EVENT_FINISHED;
    }
    break;
    case CameraMouseModes::Zoom: {
        float xDelta = event.xrel * flFrameDelta * 100;
        float yDelta = event.yrel * flFrameDelta * 100;

        m_Origin += m_vForward * (xDelta + yDelta);

        return EVENT_FINISHED;
    }
    case CameraMouseModes::None:
        break;
    default:
        break;
    }

    return 0;
}

void CameraController::SetupPerspectiveMatrix()
{
    double fov = glm::radians(m_pFov->GetFloat());

    glm::vec2 vp = m_pViewport->GetClientArea();

    float m_iWidth  = vp[0];
    float m_iHeight = vp[1];

    if (!m_iWidth || !m_iHeight)
        return;

    double aspect = m_iWidth / m_iHeight;
    double fov_y  = 2 * atan(tan(fov / 2) / aspect); // Ìîÿ ôîðìóëà - ïëàâàåò?
    double dbg    = glm::degrees(fov_y);

    m_matProjection = glm::perspective(fov_y, aspect, (double)m_pZNear->GetFloat(), (double)m_pZFar->GetFloat());

#if 0

    // Achieving orthogonal camera like in Fusion-360
    // using glm::ortho yields more or less good results,
    // although some thing should be kept in mind:
    // 1) Depth axis becomes de-facto absent,
    // 2) Moving through depth axis should be handled by zooming
    // 3) Zooming in and out should be handled by scaling ortho viewport (kinda like changing fov)
    // 4) Viewport::ScreenToWorld seems work fine and seems to output prediactable z-values in linear fashion

    glm::vec2 ext = m_pViewport->GetClientArea() / 2.f;
    m_matProjection = glm::ortho(-ext.x / 2, ext.x / 2.f, -ext.y / 2.f, ext.y / 2.f, m_pZNear->GetFloat(), m_pZFar->GetFloat());
#endif

#ifdef OLD_GL
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float *)&m_matProjection);
#endif
}

void CameraController::SetupModelViewMatrix()
{
    m_matModelView = glm::mat4(1);

#define QUAKE_STYLE

    auto concatRotate = [&](float deg, float x, float y, float z) {
        glm::mat4 ident = glm::mat4(1);
        ident           = glm::rotate(ident, glm::radians(deg), glm::vec3(x, y, z));
        m_matModelView *= ident;
    };

    concatRotate(-90, 1, 0, 0); // put Z going up
    concatRotate(90, 0, 0, 1);  // put Z going up
    concatRotate(-m_Angles[2], 1, 0, 0);
    concatRotate(-m_Angles[0], 0, 1, 0);
    concatRotate(-m_Angles[1], 0, 0, 1);

    m_matModelView = glm::translate(m_matModelView, -m_Origin);

#ifdef OLD_GL
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float *)&m_matModelView);
#endif

    auto matInv = glm::inverse(m_matModelView);

    m_vForward = -matInv[2];
    m_vRight   = matInv[0];
    m_vUp      = matInv[1];
}

CameraCommandKeyStroke::CameraCommandKeyStroke()
{
    for (int i = 0; i < 9; i++)
        m_KeyboardKeys[i] = -1;

    m_MouseKeys = 0;
}

CameraCommandKeyStroke::~CameraCommandKeyStroke()
{
}

void CameraCommandKeyStroke::AddKeyboardKey(const char *keyName)
{
    auto keyCode  = SDL_GetKeyFromName(keyName);
    auto scanCode = SDL_GetScancodeFromKey(keyCode);

    for (auto &it : m_KeyboardKeys)
    {
        if (it == -1)
        {
            it = scanCode;
            return;
        }
    }

    Application::EPICFAIL("CameraCommandKeyStroke::AddKeyboardKey(): keystroke is too long");
}

void CameraCommandKeyStroke::AddMouseKey(int keyMask)
{
    m_MouseKeys |= keyMask;
}

bool CameraCommandKeyStroke::AreKeysPressed()
{
    int nKeys = 0;

    const uint8_t *keyboardState = SDL_GetKeyboardState(&nKeys);
    const uint32_t mouseState    = SDL_GetMouseState(0, 0);

    for (auto &it : m_KeyboardKeys)
    {
        if (it == -1)
            break;

        if (!keyboardState[it])
            return false;
    }

    if (m_MouseKeys)
    {
        if (mouseState & m_MouseKeys)
            return true;
    }
    else
        return true;

    return false;
}

void CameraCommandKeyStroke::SetCallback(pfnKeyStrokeCallback callback)
{
    m_Callback = callback;
}

void CameraCommandKeyStroke::ExecuteCallback(bool bHit, const SDL_Event &event)
{
    m_Callback(bHit, event);
}

void CameraCommandKeyStroke::SetWheelDirection(int direction, bool needOtherKeys)
{
    m_WheelConditions = direction;
    m_bNeedOtherKeys  = needOtherKeys;
}

bool CameraCommandKeyStroke::CanBeExecuteFromMouseWheel(int direction)
{
    bool sameSign = (m_WheelConditions ^ direction) >= 0;

    if (m_bNeedOtherKeys)
    {
        return sameSign && AreKeysPressed();
    }
    else
        return sameSign;
}

void CameraController::DoZoomIn()
{
    m_Origin += m_vForward * std::max(glm::length(m_Origin) / 2.f, 1.f);
}

void CameraController::DoZoomOut()
{
    m_Origin -= m_vForward * std::max(glm::length(m_Origin) / 2.f, 1.f);
}
