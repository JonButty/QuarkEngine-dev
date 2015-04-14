#include "cameraFactory.h"

namespace GFX
{
    void CameraFactory::onCreateInstance( Camera* cam )
    {
        cam->Load();
    }

    void CameraFactory::onDestroyInstance( Camera* cam )
    {
        cam->Unload();
    }
}