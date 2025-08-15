#include "application.h"
#include "common.h"
#include "opengl_shared.h"

#include "variant.h"
#include "persistent.h"

#include "r_scene_renderer.h"
#include "r_editor_grid.h"


SceneRenderer::SceneRenderer()
{
    GridRenderer::Instance()->Init();

    auto pers              = Application::Instance()->GetPersistentStorage();
    m_pShowGround          = pers->GetSetting(ApplicationSettings::ShowGround);
}

SceneRenderer::~SceneRenderer()
{
    delete GridRenderer::Instance();
}

void SceneRenderer::Render(CameraController * pCamera)
{
    GLBackend::Instance()->NewFrame(pCamera->GetProjectionMatrix(), pCamera->GetViewMatrix());

    if (m_pShowGround->GetAsBool())
    {
        GridRenderer::Instance()->Render();
        GL_CheckForErrors();
    }
}