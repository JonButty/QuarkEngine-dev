#ifndef CAMERAFACTORY_H
#define CAMERAFACTORY_H

#include "factory.h"
#include "singleton.h"
#include "cameraTypes.h"
#include "camera.h"
#include "configGraphicsDLL.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT CameraFactory
        :   public Util::Factory<Camera,C_TOTAL>,
            public Util::Singleton<CameraFactory>
    {
    private:
        void onCreateInstance(Camera* cam);
        void onDestroyInstance(Camera* cam);
    };
}

#endif