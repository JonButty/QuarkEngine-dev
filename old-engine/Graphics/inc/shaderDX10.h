#ifndef SHADERDX10_H
#define SHADERDX10_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10

#include "configGraphicsDLL.h"

#include <d3d10.h>

#include <d3dx10math.h>
#include <d3dx10async.h>

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
                    const D3DXMATRIX& worldMtx,
					const D3DXMATRIX& transMtx,
                    const D3DXMATRIX& viewMtx,
                    const D3DXMATRIX& projMtx);
        void Unload();
    public:
        unsigned int Type() const;
    private:
        bool loadShaderFile(const std::string& shaderProfile,
                            const WCHAR* shaderFile,
                            ID3D10Blob*& errorMsg);
        void sendMatricies(const D3DXMATRIX& worldMtx,
						   const D3DXMATRIX& transMtx,
                           const D3DXMATRIX& viewMtx,
                           const D3DXMATRIX& projMtx);
        void printShaderErrorMessage(ID3D10Blob*& errorMessage,
            const WCHAR* shaderFilename);
    protected:
        virtual void load(ID3D10Effect* effect);
        virtual void sendVertexShaderParams();
        virtual void sendPixelShaderParams();
        virtual void unload();
    private:
        ID3D10Effect* effect_;
        ID3D10EffectTechnique* technique_;
        ID3D10InputLayout* layout_;
        ID3D10EffectMatrixVariable* modelToWorldMtxPtr_;
        ID3D10EffectMatrixVariable* modelToWorldNormalMtxPtr_;
        ID3D10EffectMatrixVariable* worldToViewMtxPtr_;
        ID3D10EffectMatrixVariable* viewToProjMtxPtr_;
        unsigned int type_;	
    };
}
#endif
#endif