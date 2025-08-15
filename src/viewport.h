
#pragma once

class Viewport
{
    int m_iArea[4];

public:
    Viewport();
    ~Viewport();

    void SetClientArea(int x, int y, int width, int height);
    void FlagUpdate();

    bool operator==(Viewport &other);

    void ApplyGL();
    glm::vec2 GetClientArea();
};