/*
    LightBaker3000 Frontend project,
    (c) 2022-2023 CrazyRussian
*/

#pragma once

class FileData;

#define MAX_UNIFORM_NAME_LEN 64

enum class ShaderTypes
{
    Vertex = 0,
    Fragment,
    Geometry,
    Compute,
    TessControl,
    TessEvaluation
};

enum class UniformKind
{
    ProjectionMatrix = 0,
    ModelViewMatrix,
    TransformMatrix,
    Scale,
    GridStep,
    GridHighlightCustom,
    GridAxisColor,
    GridMainColor,
    Grid64thLineColor,
    Grid1024thLineColor,
    GridCustomColor,
    Color,
    Color2,
    Color3,
    Color4,
    RightVector,
    UpVector,
    ForwardVector,
    Diffuse,
    Lightmap,
    Viewport,
    ObjectSerialNumber,
    BonesTransform,
    RenderMode,
};

enum class UniformDataType
{
    Float = 0,
    FloatVec2,
    FloatVec3,
    FloatVec4,
    Int,
    IntVec2,
    IntVec3,
    IntVec4,
    Bool,
    Bool2,
    Bool3,
    Bool4,
    FloatMat2,
    FloatMat3,
    FloatMat4,
    Sampler1D,
    Sampler2D,
    Sampler3D,
    SamplerCube,
    Sampler1DShadow,
    Sampler2DShadow,
    Sampler2DRect,
    Sampler2DRectShadow,
    FloatMat4Array,
};

#define FL_TEXTURE_UNIT0 (1 << 0)
#define FL_TEXTURE_UNIT1 (1 << 1)
#define FL_TEXTURE_UNIT2 (1 << 2)
#define FL_TEXTURE_UNIT3 (1 << 3)

typedef struct uniformDecl_s
{
    const char *    uniformName;
    UniformKind     kind;
    UniformDataType datatype;
    uint32_t        flags;
} uniformDecl_t;

class ShaderUniform
{
    union {
        glm::vec1  valFloat;
        glm::vec2  valFloat2;
        glm::vec3  valFloat3;
        glm::vec4  valFloat4;
        glm::ivec1 valInt;
        glm::ivec2 valInt2;
        glm::ivec3 valInt3;
        glm::ivec4 valInt4;
        glm::mat2  valMat2;
        glm::mat3  valMat3;
        glm::mat4  valMat4;

        int valSampler1D;
        int valSampler2D;
        int valSampler3D;
        int valSamplerCube;
        int valSampler1DShadow;
        int valSampler2DShadow;
        int valSampler2DRect;
        int valSampler2DRectShadow;
    } m_ValueCached;

    uniformDecl_t *m_pDecl;
    GLuint         m_Location;
    GLuint         m_ProgramId;

    void UpdateUniformValue();

public:
    ShaderUniform(uniformDecl_t *decl, GLuint location, GLuint programId);
    ~ShaderUniform();

    const char *Name() const
    {
        return m_pDecl->uniformName;
    }

    void SetFloat(float newVal);
    void SetFloat2(glm::vec2 newVal);
    void SetFloat3(glm::vec3 newVal);
    void SetFloat4(glm::vec4 newVal);
    void SetInt(int newVal);
    void SetInt2(glm::ivec2 newVal);
    void SetInt3(glm::ivec3 newVal);
    void SetInt4(glm::ivec4 newVal);
    void SetBool(glm::bvec1 newVal);
    void SetBool2(glm::bvec2 newVal);
    void SetBool3(glm::bvec3 newVal);
    void SetBool4(glm::bvec4 newVal);
    void SetMat2(glm::mat2 newVal);
    void SetMat3(glm::mat3 newVal);
    void SetMat4(glm::mat4 newVal);
    void SetMat4Array(glm::mat4 *data, size_t n);
    void SetSampler1D(int newVal);
    void SetSampler2D(int newVal);
    void SetSampler3D(int newVal);
    void SetSamplerCube(int newVal);
    void SetSampler1DShadow(int newVal);
    void SetSampler2DShadow(int newVal);
    void SetSampler2DRect(int newVal);
    void SetSampler2DRectShadow(int newVal);

    UniformKind Kind();
};

class ShaderProgram
{
protected:
    GLuint m_uiProgramId = 0;

    GLuint m_uiVertexShader   = 0;
    GLuint m_uiGeometryShader = 0;
    GLuint m_uiFragmentShader = 0;

    GLuint MakeShader(const char *fileName, GLuint type);

private:
    std::vector<ShaderUniform *> m_vecUniforms;
    void                         ParseProgramUniforms();

    std::string             PreprocessIncludes(FileData *fd);
    GLuint                  MakeShader(std::string &source, ShaderTypes type, std::list<const char *> &defines);
    std::list<const char *> m_Defines;

    void LoadAndParseShader();

    size_t m_Hash;

public:
    // Uber shader
    ShaderProgram(std::string fileName, std::list<const char *> defs);
    ShaderProgram();
    virtual ~ShaderProgram();

    size_t Hash();

    bool AttachVertexShader(const char *fileName);
    bool AttachFragmentShader(const char *fileName);
    bool AttachGeometryShader(const char *fileName);

    void LinkProgram();

    void Bind() const;
    void Unbind() const;

    std::vector<ShaderUniform *> &Uniforms();
    ShaderUniform *               UniformByKind(UniformKind kind);

    void Reload();

    static size_t CalculateHash(std::string &fileName, std::list<const char *> &defs);

private:
    std::string m_FileName;
};

//// Make clang happy
//#if defined(__clang__)
//
//#define DO_PRAGMA(x) _Pragma(#x)
//#define NOWARN(warnoption, ...)                                                                                        \
//    DO_PRAGMA(GCC diagnostic push)                                                                                     \
//    DO_PRAGMA(GCC diagnostic ignored #warnoption)                                                                      \
//    __VA_ARGS__                                                                                                        \
//    DO_PRAGMA(GCC diagnostic pop)
//
//#define BEGIN_UNIFORM_SETUP_BLOCK NOWARN(-wswitch)
//#define END_UNIFORM_SETUP_BLOCK   
//
//#else
//
//#endif
