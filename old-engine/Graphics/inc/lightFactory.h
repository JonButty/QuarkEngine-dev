#ifndef LIGHTFACTORY_H
#define LIGHTFACTORY_H

#include "factory.h"
#include "singleton.h"
#include "light.h"
#include "lightTypes.h"
#include "configGraphicsDLL.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT LightFactory
        :   public Util::Factory<Light,LT_TOTAL>,
            public Util::Singleton<LightFactory>
    {
    };
}

#endif