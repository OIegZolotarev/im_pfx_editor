/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/

#include "application.h"
#include "common.h"

#include "commands_registry.h"
#include "persistent.h"

#include "opengl_shared.h"

#include "variant.h"
#include "r_editor_grid.h"


GridRenderer::~GridRenderer()
{
    if (m_pGridMesh)
        delete m_pGridMesh;
}

void GridRenderer::Init()
{
    auto persistent = Application::GetPersistentStorage();

    m_GridAxisColor       = persistent->GetSetting(ApplicationSettings::GridAxisColor);
    m_GridMainColor       = persistent->GetSetting(ApplicationSettings::GridMainColor);
    m_Grid64thLineColor   = persistent->GetSetting(ApplicationSettings::Grid64thLineColor);
    m_Grid1024thLineColor = persistent->GetSetting(ApplicationSettings::Grid1024thLineColor);
    m_GridCustomColor     = persistent->GetSetting(ApplicationSettings::GridCustomColor);
    m_GridCustomStep      = persistent->GetSetting(ApplicationSettings::GridCustomStep);

    CreateMesh();

    
    Application::CommandsRegistry()->RegisterCommand(new CCommand(
        GlobalCommands::IncreaseGridStep, "Increase grid step", "]", 0, CMD_ONLY_TEXT, [&]() { StepUpGrid(); }));

    Application::CommandsRegistry()->RegisterCommand(new CCommand(
        GlobalCommands::DecreaseGridStep, "Decrease grid step", "[", 0, CMD_ONLY_TEXT, [&]() { StepDownGrid(); }));
    

    std::list<const char *> defs;
    m_pShader = GLBackend::Instance()->QueryShader("res/glprogs/editor_grid.glsl", defs);
}

void GridRenderer::CreateMesh()
{
    m_pGridMesh = new DrawMesh;

    int spacing     = 1;
    float grid_size = 8192;

    float min_coord = -(grid_size) / 2;
    float max_coord = -min_coord;

    m_pGridMesh->Begin(GL_TRIANGLES);

    // Quad Scheme
    // 0--1
    // | /|
    // |/ |
    // 2--3

    m_pGridMesh->Vertex3f(min_coord, min_coord, 0);
    m_pGridMesh->Vertex3f(max_coord, min_coord, 0);
    m_pGridMesh->Vertex3f(min_coord, max_coord, 0);

    m_pGridMesh->Vertex3f(max_coord, min_coord, 0);
    m_pGridMesh->Vertex3f(max_coord, max_coord, 0);
    m_pGridMesh->Vertex3f(min_coord, max_coord, 0);

    m_pGridMesh->End();
}

void GridRenderer::Render()
{

//#define USE_OLD_SHADER

#ifdef USE_OLD_SHADER
    auto shader = GLBackend::Instance()->EditorGridShader();
    shader->Bind();
    
    shader->SetTransformIdentity();
    shader->SetDefaultCamera();
    
    shader->SetGridStep(m_iGridResolutions);    
    shader->SetGridCustomHighlight(m_GridCustomStep->GetInt());
    
    shader->SetAxisColor({m_GridAxisColor->GetColorRGB().rgb, 1});
    shader->SetGridMainColor({m_GridMainColor->GetColorRGB().rgb, 1});
    shader->SetGrid64thLineColor({m_Grid64thLineColor->GetColorRGB().rgb, 1});
    shader->SetGrid1024thLineColor({m_Grid1024thLineColor->GetColorRGB().rgb, 1});
    shader->SetGridCustomColor({m_GridCustomColor->GetColorRGB().rgb, 1});
#else

    m_pShader->Bind();
    SetupShaderUniforms();

#endif

    glDepthMask(GL_FALSE);
    GLBackend::SetBlending(true);

    m_pGridMesh->BindAndDraw();
    
    GLBackend::SetBlending(false);
    glDepthMask(GL_TRUE);

#ifdef USE_OLD_SHADER
    shader->Unbind();
#else
    m_pShader->Unbind();
#endif
}

void GridRenderer::SetupShaderUniforms()
{
    for (auto it : m_pShader->Uniforms())
    {
        switch (it->Kind())
        {
        case UniformKind::TransformMatrix: {
            glm::mat4 identity = glm::mat4(1);
            it->SetMat4(identity);
        }
        break;
        case UniformKind::GridStep:
            it->SetInt(m_iGridResolution);
            break;
        case UniformKind::GridHighlightCustom:
            it->SetInt(m_GridCustomStep->GetInt());
            break;
        case UniformKind::GridAxisColor:
            it->SetFloat4({m_GridAxisColor->GetColorRGB(), 1});
            break;
        case UniformKind::GridMainColor:
            it->SetFloat4({m_GridMainColor->GetColorRGB(), 1});
            break;
        case UniformKind::Grid64thLineColor:
            it->SetFloat4({m_Grid64thLineColor->GetColorRGB(), 1});
            break;
        case UniformKind::Grid1024thLineColor:
            it->SetFloat4({m_Grid1024thLineColor->GetColorRGB(), 1});
            break;
        case UniformKind::GridCustomColor:
            it->SetFloat4({m_GridCustomColor->GetColorRGB(), 1});
            break;
        default:
            GLBackend::SetUniformValue(it);
            break;
        }
    }
}

void GridRenderer::SetGridResolution(int steps)
{
    m_iGridResolution = steps;
}

void GridRenderer::StepDownGrid()
{
    m_iGridPower       = std::max(0, --m_iGridPower);
    m_iGridResolution = 1 << m_iGridPower;
    
    // Con_Printf("Grid step: %d\n", m_iGridResolution);
    //Application::GetMainWindow()->UpdateStatusbar(1 << StatusbarField::GridStep);
    
    // ViewportsOrchestrator::Instance()->FlagRepaintAll();
}

void GridRenderer::StepUpGrid()
{
    m_iGridPower       = std::min(13, ++m_iGridPower);
    m_iGridResolution = 1 << m_iGridPower;

    // Con_Printf("Grid step: %d\n", miGridResolution);

    // Application::GetMainWindow()->UpdateStatusbar(1 << StatusbarField::GridStep);

    // ViewportsOrchestrator::Instance()->FlagRepaintAll();
}

int GridRenderer::GridStep()
{
    return m_iGridResolution;
}
