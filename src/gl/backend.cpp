#include "../common.h"
#include "../mathlib.h"

#include "backend.h"
#include "shader.h"


size_t GLBackend::m_CurrentTextureUnit;
textureUnitState_t GLBackend::m_TexturesUnitStates[16];

bool   GLBackend :: m_bBlendingEnabled = false;
GLenum GLBackend :: m_eBlendingSFactor = false;
GLenum GLBackend :: m_eBlendingDFactor = false;


GLBackend::GLBackend()
{
    ReloadAllShaders();
}

// void GLBackend::OnMeshDrawn(DrawMesh *pMesh, size_t numTriangles)
// {
//     m_RenderStats.nDrawCalls++;
//     m_RenderStats.nTriangles += numTriangles;
// }

GLBackend *GLBackend::Instance()
{
    static GLBackend *pInstance = new GLBackend;
    return pInstance;
}

GLBackend::~GLBackend()
{
    DeleteAllShaders();
    m_LoadedShaders.clear();
}

void GLBackend::DeleteAllShaders()
{
    
}

ShaderProgram *GLBackend::QueryShader(std::string fileName, std::list<const char *> defines)
{
    //BT_PROFILE("GLBackend::QueryShader()");

    ShaderProgram *result = nullptr;
    size_t hashVal        = ShaderProgram::CalculateHash(fileName, defines);

    for (auto it : m_LoadedShaders)
    {
        if (it->Hash() == hashVal)
            return it;
    }

    result = new ShaderProgram(fileName, defines);
    m_LoadedShaders.push_back(result);

    return result;
}

void GLBackend::ReloadAllShaders()
{
    DeleteAllShaders();
    
    for (auto it : m_LoadedShaders)
        it->Reload();
}

void GLBackend::NewFrame()
{

    m_RenderStats.nDrawCalls = 0;
    m_RenderStats.nTriangles = 0;


    m_RenderStats.nShaderBinds            = 0;
    m_RenderStats.nUnnecessaryShaderBinds = 0;
    m_RenderStats.idLastShader            = 0;
}

renderStats_t *GLBackend::RenderStats()
{
    return &m_RenderStats;
}

void GLBackend::BindTexture(int unit, const GLTexture *texture)
{
    auto state = &m_TexturesUnitStates[unit];

    if (!texture)
    {
        if (state->enabled)
        {
            state->enabled = false;

            if (m_CurrentTextureUnit != unit)
            {
                glActiveTexture(GL_TEXTURE0 + unit);
                m_CurrentTextureUnit = unit;
            }

            glDisable(GL_TEXTURE_2D);
        }

        return;
    }

    // TODO: implement
    // BindTexture(unit, texture->GLTextureNum());
}

void GLBackend::BindTexture(int unit, GLuint texture)
{
    auto state = &m_TexturesUnitStates[unit];

    if (m_CurrentTextureUnit != unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        m_CurrentTextureUnit = unit;
    }

    if (!state->enabled)
    {
        glEnable(GL_TEXTURE_2D);
        state->enabled = true;
    }

    if (state->texture != texture)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        state->texture = texture;
    }
}

void GLBackend::SetUniformValue(ShaderUniform *it)
{
    // TODO: implement
    // CameraController *camera = Application::GetMainWindow()->GetSceneRenderer()->GetCamera();

    switch (it->Kind())
    {
    // case UniformKind::ProjectionMatrix:
    //     it->SetMat4(camera->GetProjectionMatrix());
    //     break;
    // case UniformKind::ModelViewMatrix:
    //     it->SetMat4(camera->GetViewMatrix());
    //     break;
    // case UniformKind::Scale:
    //     it->SetFloat3({1.f, 1.f, 1.f});
    //     break;
    default:
        BREAKPOINT();
        break;
    }
}

void GLBackend::SetBlending(bool enable, GLenum sfactor, GLenum dfactor)
{
    if (m_bBlendingEnabled != enable)
    {
        m_bBlendingEnabled = enable;

        if (m_bBlendingEnabled)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }

    if (sfactor != m_eBlendingSFactor || dfactor != m_eBlendingDFactor)
    {
        m_eBlendingSFactor = sfactor;
        m_eBlendingDFactor = dfactor;

        glBlendFunc(m_eBlendingSFactor, m_eBlendingDFactor);
    }

}

void GLBackend::SetBlending(bool enable)
{
    if (m_bBlendingEnabled != enable)
    {
        m_bBlendingEnabled = enable;

        if (m_bBlendingEnabled)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }
}

static const char *GL_GetErrorString(int errorCode)
{
    switch (errorCode)
    {
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";
    default:
        return "UNKNOWN ERROR";
    }
}

void _GL_CheckForErrors(const char *filename, int line)
{
    for (int code = glGetError(); code != GL_NO_ERROR; code = glGetError())
    {
        Con_Printf("%s (at %s:%i)\n", GL_GetErrorString(code), filename, line);
    }
}
