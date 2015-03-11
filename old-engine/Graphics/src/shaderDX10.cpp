#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10

#include "shaderDX10.h"
#include "shaderTypes.h"
#include "utilAssert.h"
#include "gfxLogger.h"
#include "paths.h"
#include "vertex.h"
#include "vertexLayout.h"
#include "gfxAPIDX10.h"
#include "shaderTableDX10.h"

#include <string>

namespace GFX
{
    static const std::string& EFFECT_SHADER_PROFILE = "fx_4_0";

    Shader::Shader(unsigned int type)
        :   effect_(0),
            technique_(0),
            layout_(0),
            modelToWorldMtxPtr_(0),
            worldToViewMtxPtr_(0),
            viewToProjMtxPtr_(0),
            type_(type)
    {
    }

    Shader::Shader( const Shader& val)
        :   effect_(val.effect_),
            technique_(val.technique_),
            layout_(val.layout_),
            modelToWorldMtxPtr_(val.modelToWorldMtxPtr_),
            worldToViewMtxPtr_(val.worldToViewMtxPtr_),
            viewToProjMtxPtr_(val.viewToProjMtxPtr_),
            type_(val.type_)
    {
    }

    Shader::~Shader()
    {
    }

    void Shader::Load()
    {
        ID3D10Device* device = API::Instance().Device();
        HRESULT result;
        ID3D10Blob* errorMessage;
        D3D10_PASS_DESC passDesc;

        // Initialize the pointers this function will use to null.
        errorMessage = 0;

        // Load shader
        if(!loadShaderFile(EFFECT_SHADER_PROFILE,(SHADER_DIR_W + SHADER_TABLE[type_]).c_str(),errorMessage))
        {
            // If there was nothing in the error message then it simply could not find the shader file itself.
            GFX_LOG_W("Missing shader file: " << SHADER_TABLE[type_].c_str());

            // If the shader failed to compile it should have written something to the error message.
            printShaderErrorMessage(errorMessage, SHADER_TABLE[type_].c_str());
            ENGINE_ASSERT(false);
        }

        // Get a pointer to the technique inside the shader.
        technique_ = effect_->GetTechniqueByName("RenderPass");
        if(!technique_)
        {
            // If there was nothing in the error message then it simply could not find the shader file itself.
            GFX_LOG_W(SHADER_TABLE[type_].c_str() << ": Invalid technique.");
            ENGINE_ASSERT(false);
        }

        // Get the description of the first pass described in the shader technique.
        technique_->GetPassByIndex(0)->GetDesc(&passDesc);

        result = device->CreateInputLayout(VertexLayout,
                                           VertexLayoutElementCount,
                                           passDesc.pIAInputSignature,
                                           passDesc.IAInputSignatureSize, 
                                           &layout_);

        ENGINE_ASSERT_MSG(!FAILED(result),"Failed to create vertex input layout");

        modelToWorldMtxPtr_ = effect_->GetVariableByName("ModelToWorldMatrix")->AsMatrix();
        modelToWorldNormalMtxPtr_ = effect_->GetVariableByName("ModelToWorldNormalMatrix")->AsMatrix();
        worldToViewMtxPtr_ = effect_->GetVariableByName("WorldToViewMatrix")->AsMatrix();
        viewToProjMtxPtr_ = effect_->GetVariableByName("ViewToProjMatrix")->AsMatrix();
        load(effect_);
    }

    void Shader::Render( int indexCount,
                         const D3DXMATRIX& worldMtx,
						 const D3DXMATRIX& transMtx,
                         const D3DXMATRIX& viewMtx,
                         const D3DXMATRIX& projMtx )
    {
        D3D10_TECHNIQUE_DESC techniqueDesc;
        ID3D10Device* device = API::Instance().Device();
        sendMatricies(worldMtx,transMtx,viewMtx,projMtx);
        sendVertexShaderParams();
        sendPixelShaderParams();

        // Set the vertex input layout.
        device->IASetInputLayout(layout_);

        technique_->GetDesc(&techniqueDesc);

        for(unsigned int i = 0; i < techniqueDesc.Passes; ++i)
        {
            technique_->GetPassByIndex(i)->Apply(0);
            device->DrawIndexed(indexCount, 0, 0);
        }
    }

    void Shader::Unload()
    {
        unload();

        // Release the pointers to the matrices inside the shader.
        modelToWorldMtxPtr_ = 0;
        worldToViewMtxPtr_ = 0;
        viewToProjMtxPtr_ = 0;

        // Release the pointer to the shader layout.
        if(layout_)
        {
            layout_->Release();
            layout_ = 0;
        }

        // Release the pointer to the shader technique.
        technique_ = 0;

        // Release the pointer to the shader.
        if(effect_)
        {
            effect_->Release();
            effect_ = 0;
        }
    }

    //--------------------------------------------------------------------------

    unsigned int Shader::Type() const
    {
        return type_;
    }

    //--------------------------------------------------------------------------

    bool Shader::loadShaderFile( const std::string& shaderProfile,
                                 const WCHAR* shaderFile,
                                 ID3D10Blob*& errorMsg)
    {
        // Compile the vertex shader code.
        HRESULT result = D3DX10CreateEffectFromFile(shaderFile,
                                                    NULL,
                                                    NULL,
                                                    shaderProfile.c_str(),
                                                    D3D10_SHADER_ENABLE_STRICTNESS,
                                                    0, 
                                                    API::Instance().Device(),
                                                    NULL,
                                                    NULL,
                                                    &effect_,
                                                    &errorMsg,
                                                    NULL);
        if(FAILED(result))
            return false;
        return true;
    }

    void Shader::sendMatricies( const D3DXMATRIX& modelToWorldMtx,
								const D3DXMATRIX& transMtx,
                                const D3DXMATRIX& worldToViewMtx,
                                const D3DXMATRIX& viewToProjMtx)
    {
		D3DXMATRIX modelToWorldTranspose;
		D3DXMatrixTranspose(&modelToWorldTranspose,&modelToWorldMtx);

		D3DXMATRIX modelToWorldTranslate;
		D3DXMatrixTranspose(&modelToWorldTranslate,&modelToWorldMtx);
		D3DXMatrixMultiply(&modelToWorldTranslate,&transMtx, &modelToWorldMtx);

        modelToWorldNormalMtxPtr_->SetMatrix((float*)&modelToWorldTranspose);
        modelToWorldMtxPtr_->SetMatrix((float*)&modelToWorldTranslate);
        worldToViewMtxPtr_->SetMatrix((float*)&worldToViewMtx);
        viewToProjMtxPtr_->SetMatrix((float*)&viewToProjMtx);
    }

    void Shader::printShaderErrorMessage( ID3D10Blob*& errorMessage,
                                          const WCHAR* shaderFilename )
    {
        char* compileErrors;
        unsigned long bufferSize;

        // Get a pointer to the error message text buffer.
        compileErrors = (char*)(errorMessage->GetBufferPointer());

        // Get the length of the message.
        bufferSize = errorMessage->GetBufferSize();

        std::stringstream ss;
        GFX_LOG_W(shaderFilename);

        // Write out the error message.
        for(unsigned int i = 0; i < bufferSize; ++i)
            ss << compileErrors[i];
        GFX_LOG(ss.str());
        ENGINE_ASSERT(false);

        // Release the error message.
        errorMessage->Release();
        errorMessage = 0;
    }

    //--------------------------------------------------------------------------

    void Shader::load(ID3D10Effect*)
    {
    }

    void Shader::sendVertexShaderParams()
    {
    }

    void Shader::sendPixelShaderParams()
    {
    }

    void Shader::unload()
    {
    }
}
#endif