#ifndef SHADERNORMALS_H
#define SHADERNORMALS_H

#include "shader.h"
#include "factory.h"
#include "configGraphicsDLL.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT ShaderNormals
        :   public Shader
    {
    public:
        ShaderNormals();
    private:
        static Util::FactoryPlant<Shader,ShaderNormals> factoryPlant_;
    };
}

#endif