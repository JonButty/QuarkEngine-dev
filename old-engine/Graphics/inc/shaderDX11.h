#ifndef SHADERDX11_H
#define SHADERDX11_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_11

#include "configGraphicsDLL.h"

#include <d3d11.h>
#include <DirectXMath.h>

#include <vector>

namespace GFX
{
    // TODO: implement uni directional translation table to map a shader type to
    // the file names of the vert and frag shaders
    class ENGINE_GRAPHICS_EXPORT Shader
    {
    public:
        Shader(unsigned int type);
        Shader(const Shader& shader);
        virtual ~Shader();
        void Load();
        void Render(int indexCount,
                    D3DXMATRIX& worldMtx,
                    D3DXMATRIX& viewMtx,
                    D3DXMATRIX& projMtx);
        void Unload();
    public:
        unsigned int Type() const;
    private:
        bool loadShaderFile(const std::string& shaderProfile,
                            const WCHAR* shaderFile,
                            ID3D10Blob*& shaderBuffer,
                            ID3D10Blob*& errorMsg);
        void sendMatricies(ID3D11DeviceContext* deviceContext,
                           D3DXMATRIX& worldMtx,
                           D3DXMATRIX& viewMtx,
                           D3DXMATRIX& projMtx,
                           unsigned int bufferNumber);
        void printShaderErrorMessage(ID3D10Blob*& errorMessage,
                                     const WCHAR* shaderFilename);
    protected:
        virtual void load(ID3D11Device* device);
        virtual void sendVertexShaderParams(ID3D11DeviceContext*& deviceContext,
                                            unsigned int startingBufferNumber);
        virtual void sendPixelShaderParams(ID3D11DeviceContext*& deviceContext,
                                           unsigned int startingBufferNumber);
        virtual void unload();
    private:
        struct MatrixBufferType
        {
            D3DXMATRIX world;
            D3DXMATRIX view;
            D3DXMATRIX projection;
            D3DXMATRIX normal;
        };
    private:
        ID3D11VertexShader* vertShader_;
        ID3D11PixelShader* pixelShader_;
        ID3D11InputLayout* layout_;
        ID3D11Buffer* mtxBuffer_;
        unsigned int type_;	
    };
}

#endif
#endif