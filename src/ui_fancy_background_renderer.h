#pragma once



#include "variant.h"
class FancyBackgroundRenderer
{    
    VariantValue *m_pBackgroudColorSetting1;
    VariantValue *m_pBackgroudColorSetting2;
    VariantValue *m_pUseGradientBackground;

    ShaderProgram *m_pBackgroundShader;
    DrawMesh *     m_pBackgroundMesh;

public:

    FancyBackgroundRenderer();
    ~FancyBackgroundRenderer();

    void PerformRendering();

};