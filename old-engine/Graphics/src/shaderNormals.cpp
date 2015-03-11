#include "shaderNormals.h"
#include "shaderTypes.h"
#include "shaderFactory.h"

namespace GFX
{
    Util::FactoryPlant<Shader,ShaderNormals> GFX::ShaderNormals::factoryPlant_(ShaderFactory::InstancePtr(),ST_NORMALS);

    ShaderNormals::ShaderNormals()
        :   Shader(ST_NORMALS)
    {
    }
}