
#include "r_camera_controller.h"

class SceneRenderer 
{
    VariantValue *m_pShowGround;


public:

    SceneRenderer();
    ~SceneRenderer();

    void Render(CameraController * pCamera); 

};