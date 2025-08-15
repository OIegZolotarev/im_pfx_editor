#include "application.h"
#include "common.h"

#include "opengl_shared.h"
#include "viewport.h"


Viewport::Viewport()
{

}

Viewport::~Viewport()
{

}


void Viewport::SetClientArea(int x, int y, int width, int height)
{
    m_iArea[0] = x;
    m_iArea[1] = y;
    m_iArea[2] = width;
    m_iArea[3] = height;
}

void Viewport::FlagUpdate()
{
    (void)0;
}

bool Viewport::operator==(Viewport &other)
{
    for (int i = 0; i < 4; i++)
    {
        if (m_iArea[i] != other.m_iArea[i])
            return false;
    }

    return true;
}

void Viewport::ApplyGL()
{
    glViewport(m_iArea[0], m_iArea[1], m_iArea[2], m_iArea[3]);
}

glm::vec2 Viewport::GetClientArea()
{
    return {(float)m_iArea[2], (float)m_iArea[3]};
}