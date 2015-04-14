#ifndef SHADERBOUNDARYDX10_H
#define SHADERBOUNDARYDX10_H

#include "shader.h"
#include "configGraphicsDLL.h"
#include "color.h"
#include "factory.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT ShaderBoundary
        :   public Shader
    {
    public:
        ShaderBoundary();
        ShaderBoundary(const ShaderBoundary& shader);
    public:
        Color BoundaryColor() const;
        void BoundaryColor(const Color& val);
    private:
        void load(ID3D10Effect* effect);
        void sendPixelShaderParams();
    private:
        ID3D10EffectVectorVariable* edgeColorPtr_;
        Color boundaryColor_;
        static Util::FactoryPlant<Shader,ShaderBoundary> factoryPlant_;
    };
}

#endif