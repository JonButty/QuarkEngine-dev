#ifndef SHADERPHONGDX10_H
#define SHADERPHONGDX10_H

#include "color.h"
#include "shaderDX10.h"
#include "factory.h"
#include "vector3.h"
#include "lightManager.h"
#include "configGraphicsDLL.h"
#include "shaderFactory.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT ShaderPhong
        :   public Shader
    {
    public:
        ShaderPhong();
        ShaderPhong(const ShaderPhong& val);
    private:
        void load(ID3D10Effect* effect);
        void sendVertexShaderParams();
        void sendPixelShaderParams();
    private:
        ID3D10EffectVectorVariable* cameraPosPtr_;
        ID3D10EffectVectorVariable* ambientColorPtr_;
        ID3D10EffectVectorVariable* diffuseColorPtr_;
        ID3D10EffectVectorVariable* lightPosPtr_;
        ID3D10EffectScalarVariable* specularPowerPtr_;
        ID3D10EffectVectorVariable* specularColorPtr_;
        LightHandle light_;
        static Util::FactoryPlant<Shader,ShaderPhong> factoryPlant_;
    };
}

#endif