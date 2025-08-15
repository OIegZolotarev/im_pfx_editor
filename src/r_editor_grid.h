/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/

#pragma once



class GridRenderer : public Singleton<GridRenderer>
{
    DrawMesh *m_pGridMesh = nullptr;
    
    int m_iGridResolution = 32;
    int m_iGridPower       = 5;
    
    VariantValue* m_GridAxisColor;
    VariantValue* m_GridMainColor;
    VariantValue* m_Grid64thLineColor;
    VariantValue* m_Grid1024thLineColor;
    VariantValue* m_GridCustomColor;
    VariantValue* m_GridCustomStep;
    
    ShaderProgram *m_pShader;

  public:
    ~GridRenderer();

    void Init();

    void CreateMesh();

    void Render();

    void SetupShaderUniforms();

    void SetGridResolution(int steps);

    void StepDownGrid();    
    void StepUpGrid();
    int  GridStep();
};
