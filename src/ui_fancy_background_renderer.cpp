#include "common.h"
#include "opengl_shared.h"
#include "application.h"

#include "variant.h"
#include "persistent.h"

#include "ui_fancy_background_renderer.h"

FancyBackgroundRenderer::FancyBackgroundRenderer()
{
    m_pBackgroudColorSetting1 = Application::GetPersistentStorage()->GetSetting(ApplicationSettings::BackgroundColor1);
    m_pBackgroudColorSetting2 = Application::GetPersistentStorage()->GetSetting(ApplicationSettings::BackgroundColor2);
    m_pUseGradientBackground =
        Application::GetPersistentStorage()->GetSetting(ApplicationSettings::UseGradientBackground);

    m_pBackgroundShader = GLBackend::Instance()->QueryShader("res/glprogs/background.glsl", {});

    m_pBackgroundMesh = new DrawMesh();

    m_pBackgroundMesh->Begin(GL_TRIANGLE_STRIP);
    m_pBackgroundMesh->Color3f(1, 0, 0);
    m_pBackgroundMesh->Vertex2f(-1, -1);

    m_pBackgroundMesh->Color3f(1, 0, 0);
    m_pBackgroundMesh->Vertex2f(+1, -1);

    m_pBackgroundMesh->Color3f(0, 0, 0);
    m_pBackgroundMesh->Vertex2f(-1, +1);

    m_pBackgroundMesh->Color3f(0, 0, 0);
    m_pBackgroundMesh->Vertex2f(+1, +1);

    m_pBackgroundMesh->End();

}

FancyBackgroundRenderer::~FancyBackgroundRenderer()
{
    delete m_pBackgroundMesh;
}

void FancyBackgroundRenderer::PerformRendering()
{
    auto col1 = m_pBackgroudColorSetting1->GetColorRGB();
    auto col2 = m_pBackgroudColorSetting2->GetColorRGB();

    glClearColor(col1[0], col1[1], col1[2], 1);
    glClear(GL_DEPTH_BUFFER_BIT);
    GL_CheckForErrors();

    if (m_pUseGradientBackground->GetAsBool())
    {
        glDepthMask(GL_FALSE);
        GL_CheckForErrors();

        m_pBackgroundShader->Bind();
        m_pBackgroundMesh->Bind();
        
        for (auto &it : m_pBackgroundShader->Uniforms())
        {
            switch (it->Kind())
            {
            case UniformKind::Color:
                it->SetFloat4({col1.xyz(), 1});
                GL_CheckForErrors();
                break;
            case UniformKind::Color2:
                it->SetFloat4({col2.xyz(), 1});
                GL_CheckForErrors();
                break;
            default:
                BREAKPOINT();
                break;
            }
        }

        GL_CheckForErrors();
        m_pBackgroundMesh->Draw();
        GL_CheckForErrors();

        glDepthMask(GL_TRUE);
        GL_CheckForErrors();
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT);
        GL_CheckForErrors();
    }
}