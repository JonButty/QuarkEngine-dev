#include "shaderDefault.h"
#include "shaderTypes.h"
#include "shaderFactory.h"

namespace GFX
{
    Util::FactoryPlant<Shader,ShaderDefault> ShaderDefault::factoryPlant_(ShaderFactory::InstancePtr(),ST_DEFAULT);

    ShaderDefault::ShaderDefault()
        :   Shader(ST_DEFAULT)
    {
    }
}