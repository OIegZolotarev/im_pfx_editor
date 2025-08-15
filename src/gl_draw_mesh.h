/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/



#pragma once

typedef struct drawVert_s
{
    glm::vec3 xyz;
    glm::vec3 normal;
    glm::vec3 tangent;
    union {
        glm::vec4 color;
        byte      bones[4];
        int       partId;
    } ext;

    glm::vec3 uv;
} drawVert_t;

typedef struct drawVertSkeletal_s
{
    glm::vec3 xyz;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 uv;
    byte      bones[4];
} drawVertSkeletal_s;

enum DrawMeshFlags
{
    None        = 0,
    HasIndices  = (1 << 2),
    Dynamic     = (1 << 3),
    UsingColors = (1 << 4),
    UsingBones  = (1 << 5),
    UsingPartId  = (1 << 6)
};

class DrawMesh
{
    GLuint m_vboId     = 0;
    GLuint m_vaoId     = 0;
    GLuint m_indBuffId = 0;
    GLenum m_drawMode  = 0;

    GLenum                    m_IndiciesType = 0;
    std::vector<unsigned int> m_Indices;
    size_t                    m_NumIndices = 0;

    drawVert_t              m_tempVert;
    std::vector<drawVert_t> m_Data;
    size_t                  m_NumElements = 0;

    int m_iFlags = 0;

public:
    DrawMesh(int flags = DrawMeshFlags::None);
    ~DrawMesh();

    void Bind();
    void Draw(uint32_t first = 0, uint32_t num = 0);
    void Unbind();

    void Begin(GLenum mode, bool draft = false);
    void End();

    void Vertex2f(float x, float y);
    void Vertex2fv(float *v);

    void Vertex3f(float x, float y, float z);
    void Vertex3fv(float *v);

    void Normal3f(float nx, float ny, float nz);
    void Normal3fv(float *v);

    void Tangent3f(float tx, float ty, float tz);
    void Tangent3fv(float *v);

    void Color4f(float r, float g, float b, float a);
    void Color4fv(float *v);

    void Color3f(float r, float g, float b);
    void Color3fv(float *v);

    void Bone1ub(size_t index,byte bone);
    void PartId(int partId);
    

    void TexCoord2f(float u, float v);
    void TexCoord2fv(float *v);

    void     Element1i(size_t idx);
    uint32_t FirstElement(size_t offset);
    uint32_t LastElement(size_t offset);

    size_t CurrentIndex();
    size_t CurrentElement();

    void BindAndDraw();
    void Element1iv(int *vals, size_t count);
    void ReverseTrianglesOrder();
};
