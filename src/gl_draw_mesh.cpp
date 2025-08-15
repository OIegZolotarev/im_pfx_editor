/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/

#include "common.h"
#include "opengl_shared.h"
#include "mathlib.h"

#include "gl_draw_mesh.h"
#include "gl_backend.h"

DrawMesh::DrawMesh(int flags)
{    
    m_iFlags = flags;
    memset(&m_tempVert, 0, sizeof(m_tempVert));    
}

DrawMesh::~DrawMesh()
{
    if (m_vboId != 0)
        glDeleteBuffers(1, &m_vboId);

    FreeVector(m_Data);
}

void DrawMesh::Bind()
{
    glBindVertexArray(m_vaoId);
}

void DrawMesh::Draw(uint32_t first /*= 0*/, uint32_t num /*= 0*/)
{
    GLBackend::Instance()->OnMeshDrawn(this, num > 0 ? num : m_NumElements);

    if (m_iFlags & HasIndices)
    {
        assert(first == 0);

        size_t nElements = num > 0 ? num : m_NumIndices;
        glDrawElements(m_drawMode, (uint32_t)nElements, m_IndiciesType, 0);
    }
    else
    {
        size_t nElements = num > 0 ? num : m_NumElements;
        glDrawArrays(m_drawMode, (uint32_t)first, (uint32_t)nElements);
    }
}

void DrawMesh::Unbind()
{
    // glBindVertexArray(0);
}

void DrawMesh::Begin(GLenum mode, bool draft)
{
    m_drawMode = mode;

    if (!draft)
    {
        if (!m_vboId)
            glGenBuffers(1, &m_vboId);

        if (!m_vaoId)
            glGenVertexArrays(1, &m_vaoId);
    }
}

void DrawMesh::End()
{
    GL_CheckForErrors();

    if (!m_vboId)
        glGenBuffers(1, &m_vboId);

    if (!m_vaoId)
        glGenVertexArrays(1, &m_vaoId);

    glBindVertexArray(m_vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
    
    GLenum use = GL_STATIC_DRAW;

    if (m_iFlags & DrawMeshFlags::Dynamic)
        use = GL_DYNAMIC_COPY;

    glBufferData(GL_ARRAY_BUFFER, sizeof(drawVert_t) * m_Data.size(), m_Data.data(), use);
    GL_CheckForErrors();
    

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(drawVert_t), (void *)offsetof(drawVert_t, xyz));
    glEnableVertexAttribArray(0);
    GL_CheckForErrors();

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(drawVert_t), (void *)offsetof(drawVert_t, normal));
    glEnableVertexAttribArray(1);
    GL_CheckForErrors();

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(drawVert_t), (void *)offsetof(drawVert_t, tangent));
    glEnableVertexAttribArray(2);
    GL_CheckForErrors();

    if (m_iFlags & UsingBones)
    {
        glVertexAttribIPointer(3, 4, GL_UNSIGNED_BYTE, sizeof(drawVert_t), (void *)offsetof(drawVert_t, ext.color));
        glEnableVertexAttribArray(3);
        GL_CheckForErrors();
    }
    else if (m_iFlags & UsingColors)
    {
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(drawVert_t), (void *)offsetof(drawVert_t, ext.color));
        glEnableVertexAttribArray(3);
        GL_CheckForErrors();
    }
    else if (m_iFlags & UsingPartId)
    {
        glVertexAttribIPointer(3, 1, GL_INT, sizeof(drawVert_t), (void *)offsetof(drawVert_t, ext.color));
        glEnableVertexAttribArray(3);
        GL_CheckForErrors();
    }


    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(drawVert_t), (void *)offsetof(drawVert_t, uv));
    glEnableVertexAttribArray(4);
    GL_CheckForErrors();

    m_NumElements = m_Data.size();
    FreeVector(m_Data);

    if (m_Indices.size() > 0)
    {
        m_iFlags |= HasIndices;

        glGenBuffers(1, &m_indBuffId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indBuffId);
        GL_CheckForErrors();


        if (m_NumElements < 0xFFFF)
        {
            std::vector<unsigned short> ushort_buffer;
            ushort_buffer.reserve(m_Indices.size());

            for (size_t i = 0; i < m_Indices.size(); i++)
                ushort_buffer.push_back((unsigned short)m_Indices[i]);

            m_IndiciesType = GL_UNSIGNED_SHORT;
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, ushort_buffer.size() * sizeof(unsigned short), ushort_buffer.data(),
                         GL_STATIC_DRAW);
        }
        else
        {
            m_IndiciesType = GL_UNSIGNED_INT;
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned short), m_Indices.data(),
                         GL_STATIC_DRAW);
        }

        GL_CheckForErrors();

        m_NumIndices = m_Indices.size();
        m_Indices.clear();
    }

    glBindVertexArray(0);
}

void DrawMesh::Vertex2f(float x, float y)
{
    m_tempVert.xyz[0] = x;
    m_tempVert.xyz[1] = y;
    m_tempVert.xyz[2] = 0;

    m_Data.push_back(m_tempVert);
}

void DrawMesh::Vertex2fv(float *v)
{
    m_tempVert.xyz[0] = v[0];
    m_tempVert.xyz[1] = v[1];
    m_tempVert.xyz[2] = 0;

    m_Data.push_back(m_tempVert);
}

void DrawMesh::Vertex3f(float x, float y, float z)
{
    m_tempVert.xyz[0] = x;
    m_tempVert.xyz[1] = y;
    m_tempVert.xyz[2] = z;

    m_Data.push_back(m_tempVert);
}

void DrawMesh::Vertex3fv(float *v)
{
    m_tempVert.xyz[0] = v[0];
    m_tempVert.xyz[1] = v[1];
    m_tempVert.xyz[2] = v[2];

    m_Data.push_back(m_tempVert);
}

void DrawMesh::Normal3f(float nx, float ny, float nz)
{
    m_tempVert.normal[0] = nx;
    m_tempVert.normal[1] = ny;
    m_tempVert.normal[2] = nz;
}

void DrawMesh::Normal3fv(float *v)
{
    m_tempVert.normal[0] = v[0];
    m_tempVert.normal[1] = v[1];
    m_tempVert.normal[2] = v[2];
}

void DrawMesh::Tangent3f(float tx, float ty, float tz)
{
    m_tempVert.tangent[0] = tx;
    m_tempVert.tangent[1] = ty;
    m_tempVert.tangent[2] = tz;
}

void DrawMesh::Tangent3fv(float *v)
{
    m_tempVert.tangent[0] = v[0];
    m_tempVert.tangent[1] = v[1];
    m_tempVert.tangent[2] = v[2];
}

void DrawMesh::Color4f(float r, float g, float b, float a)
{
    assert(!(m_iFlags & UsingBones));
    assert(!(m_iFlags & UsingPartId));

    m_tempVert.ext.color[0]   = r;
    m_tempVert.ext.color[1]  = g;
    m_tempVert.ext.color[2]  = b;
    m_tempVert.ext.color[3] = a;

    m_iFlags |= UsingColors;
}

void DrawMesh::Color4fv(float *v)
{
    assert(!(m_iFlags & UsingBones));
    assert(!(m_iFlags & UsingPartId));

    m_tempVert.ext.color[0]   = v[0];
    m_tempVert.ext.color[1]  = v[1];
    m_tempVert.ext.color[2]  = v[2];
    m_tempVert.ext.color[3] = v[3];

    m_iFlags |= UsingColors;
}

void DrawMesh::Color3f(float r, float g, float b)
{
    assert(!(m_iFlags & UsingBones));
    assert(!(m_iFlags & UsingPartId));

    m_tempVert.ext.color[0]   = r;
    m_tempVert.ext.color[1]  = g;
    m_tempVert.ext.color[2]  = b;
    m_tempVert.ext.color[3] = 1;

    m_iFlags |= UsingColors;
}

void DrawMesh::Color3fv(float *v)
{
    assert(!(m_iFlags & UsingBones));
    assert(!(m_iFlags & UsingPartId));

    m_tempVert.ext.color[0]   = v[0];
    m_tempVert.ext.color[1]  = v[1];
    m_tempVert.ext.color[2]  = v[2];
    m_tempVert.ext.color[3] = 1;

    m_iFlags |= UsingColors;
}

void DrawMesh::Bone1ub(size_t index, byte bone)
{
    assert(!(m_iFlags & UsingColors));
    assert(!(m_iFlags & UsingPartId));
    assert(index < 4);

    m_tempVert.ext.bones[index] = bone;

    m_iFlags |= UsingBones;
}

void DrawMesh::PartId(int partId)
{
    assert(!(m_iFlags & UsingColors));
    assert(!(m_iFlags & UsingBones));

    m_tempVert.ext.partId = partId;

    m_iFlags |= UsingPartId;
}

void DrawMesh::TexCoord2f(float u, float v)
{
    m_tempVert.uv[0] = u;
    m_tempVert.uv[1] = v;
}

void DrawMesh::TexCoord2fv(float *v)
{
    m_tempVert.uv[0] = v[0];
    m_tempVert.uv[1] = v[1];
}

void DrawMesh::Element1i(size_t idx)
{
    m_Indices.push_back((uint32_t)idx);
}

uint32_t DrawMesh::FirstElement(size_t offset)
{
    return m_Indices[offset];
}

uint32_t DrawMesh::LastElement(size_t offset)
{
    return m_Indices[(m_Indices.size() - 1) - offset];
}

size_t DrawMesh::CurrentIndex()
{
    return m_Indices.size();
}

size_t DrawMesh::CurrentElement()
{
    return m_Data.size();
}

void DrawMesh::BindAndDraw()
{
    Bind();
    Draw();
    Unbind();
}

void DrawMesh::Element1iv(int *val, size_t count)
{
    for (size_t i = 0; i < count; i++)
        m_Indices.push_back(val[i]);
}

void DrawMesh::ReverseTrianglesOrder()
{
    typedef struct tri_s
    {
        int ind[3];
    }tri_t;

    tri_s *tris = (tri_s*)m_Indices.data();
    size_t nTris = m_Indices.size() / 3;

    for (size_t i = 0 ; i < nTris; i++)
    {
        int t = tris[i].ind[0];
        tris[i].ind[0] = tris[i].ind[2];
        tris[i].ind[2] = t;
    }


}



