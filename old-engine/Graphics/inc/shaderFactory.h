#ifndef SHADERFACTORY_H
#define SHADERFACTORY_H

#include "factory.h"
#include "singleton.h"
#include "shaderTypes.h"
#include "shader.h"
#include "configGraphicsDLL.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT ShaderFactory
        :   public Util::Factory<Shader,ST_TOTAL>,
            public Util::Singleton<ShaderFactory>
    {
    public:
        void OnDestroyInstance(Shader* shader)
        {
            shader->Unload();
        }
    };
}

#endif