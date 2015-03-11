#ifndef SHADERDEFAULT_H
#define SHADERDEFAULT_H

#include "shader.h"
#include "factory.h"
#include "configGraphicsDLL.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT ShaderDefault
        :   public Shader
    {
    public:
        ShaderDefault();
    private:
        static Util::FactoryPlant<Shader,ShaderDefault> factoryPlant_;
    };
}

#endif