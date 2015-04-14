#ifndef SHADERPHONGDX11_H
#define SHADERPHONGDX11_H

#include "color.h"
#include "shaderDX11.h"
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
        void load(ID3D11Device* device);
        void sendVertexShaderParams(ID3D11DeviceContext*& deviceContext,
                                    unsigned int startingBufferNumber);
        void sendPixelShaderParams(ID3D11DeviceContext*& deviceContext,
                                   unsigned int startingBufferNumber);
        void unload();
    private:
        struct CameraBufferType
        {
            D3DXVECTOR3 cameraPosition;
            float padding;
        };
        struct LightBufferType
        {
            D3DXVECTOR4 ambientColor;
            D3DXVECTOR4 diffuseColor;
            D3DXVECTOR3 lightPos;
            float specularPower;
            D3DXVECTOR4 specularColor;
        };
    private:
        ID3D11Buffer* cameraBuffer_;
        ID3D11Buffer* lightBuffer_;
        LightHandle light_;
        static Util::FactoryPlant<Shader,ShaderPhong> factoryPlant_;
    };
}

#endif