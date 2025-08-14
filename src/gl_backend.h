
#include <stdlib.h>
#include "opengl_shared.h"

class GLTexture;
class ShaderProgram;
class ShaderUniform;

typedef struct renderStats_s
{
    size_t nDrawCalls = 0;
    size_t nTriangles = 0;

    size_t nEntitiesRendered = 0;
    size_t nEntitiesTotal = 0;

    size_t nShaderBinds = 0;
    size_t nUnnecessaryShaderBinds = 0;
    size_t idLastShader = 0;

} renderStats_t;

typedef struct textureUnitState_s
{
    bool enabled = true;
    GLuint texture = 0;
} textureUnitState_t;

class GLBackend
{
    GLBackend();


    // Rendering stats;
    renderStats_t m_RenderStats;
    // void OnMeshDrawn(DrawMesh *pMesh, size_t numTriangles);

    // Texture units
    static size_t m_CurrentTextureUnit;
    static textureUnitState_t m_TexturesUnitStates[16];    

    // Blending state
    static bool m_bBlendingEnabled;
    static GLenum m_eBlendingSFactor;
    static GLenum m_eBlendingDFactor;

    // Shaders
    std::list<ShaderProgram *> m_LoadedShaders;

public:
    static GLBackend *Instance();
    ~GLBackend();

    // Shader managment
    ShaderProgram *QueryShader(std::string fileName, std::list<const char*> defines);
    void DeleteAllShaders();
    void ReloadAllShaders();

    void NewFrame();
    renderStats_t *RenderStats();

    static void BindTexture(int unit, const GLTexture *texture);
    static void BindTexture(int unit, GLuint texture);
    static void SetUniformValue(ShaderUniform *it);

    static void SetBlending(bool enable, GLenum sfactor, GLenum dfactor);
    static void SetBlending(bool enable);
};


void _GL_CheckForErrors(const char *filename, int line);

// #define GL_DEBUG
#ifdef GL_DEBUG
#define GL_CheckForErrors() _GL_CheckForErrors(__FILE__, __LINE__)
#else
#define GL_CheckForErrors() (void)0;
#endif