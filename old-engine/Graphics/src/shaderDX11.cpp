#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_11

#include "gfxAPIDX11.h"
#include "shaderDX11.h"
#include "shaderTypes.h"
#include "utilAssert.h"
#include "gfxLogger.h"
#include "paths.h"
#include "vertex.h"
#include "vertexLayoutDX11.h"
#include "shaderTableDX11.h"

#include <string>

namespace GFX
{
    static const std::string& VERTEX_SHADER_PROFILE = "vs_5_0";
    static const std::string& PIXEL_SHADER_PROFILE = "ps_5_0";

    Shader::Shader(unsigned int type)
        :   vertShader_(0),
            pixelShader_(0),
            layout_(0),
            mtxBuffer_(0),
            type_(type)
    {
    }

    Shader::Shader( const Shader& val)
        :   vertShader_(val.vertShader_),
            pixelShader_(val.pixelShader_),
            layout_(val.layout_),
            mtxBuffer_(val.mtxBuffer_),
            type_(val.type_)
    {
    }

    Shader::~Shader()
    {
    }

    void Shader::Load()
    {
        ID3D11Device* device = API::Instance().Device();
        HRESULT result;
        ID3D10Blob* errorMessage;
        ID3D10Blob* vertexShaderBuffer;
        ID3D10Blob* pixelShaderBuffer;

        // Initialize the pointers this function will use to null.
        errorMessage = 0;
        vertexShaderBuffer = 0;
        pixelShaderBuffer = 0;

        // Load vertex shader
        if(!loadShaderFile(VERTEX_SHADER_PROFILE,(VERTEX_SHADER_DIR_W + SHADER_TABLE[type_].first).c_str(), vertexShaderBuffer,errorMessage))
        {
            // If there was nothing in the error message then it simply could not find the shader file itself.
            ENGINE_ASSERT_MSG(errorMessage,"Missing shader file: " << SHADER_TABLE[type_].first.c_str());
            
            // If the shader failed to compile it should have written something to the error message.
            printShaderErrorMessage(errorMessage, SHADER_TABLE[type_].first.c_str());
            ENGINE_ASSERT(false);
        }

        // Load pixel shader
        if(!loadShaderFile(PIXEL_SHADER_PROFILE,(PIXEL_SHADER_DIR_W + SHADER_TABLE[type_].second).c_str(), pixelShaderBuffer,errorMessage))
        {
            // If there was nothing in the error message then it simply could not find the shader file itself.
            ENGINE_ASSERT_MSG(errorMessage,"Missing shader file: " << SHADER_TABLE[type_].second.c_str());

            // If the shader failed to compile it should have written something to the error message.
            printShaderErrorMessage(errorMessage, SHADER_TABLE[type_].second.c_str());
            ENGINE_ASSERT(false);
        }

        // Create the vertex shader from the buffer.
        result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertShader_);
        ENGINE_ASSERT_MSG(!FAILED(result),"Failed to create vertex shader for " << SHADER_TABLE[type_].first.c_str());

        // Create the pixel shader from the buffer.
        result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader_);
        ENGINE_ASSERT_MSG(!FAILED(result),"Failed to create pixel shader for " << SHADER_TABLE[type_].second.c_str());

        result = device->CreateInputLayout(VertexLayout,
                                           VertexLayoutElementCount,
                                           vertexShaderBuffer->GetBufferPointer(), 
                                           vertexShaderBuffer->GetBufferSize(),
                                           &layout_);

        ENGINE_ASSERT_MSG(!FAILED(result),"Failed to create vertex input layout");

        // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
        vertexShaderBuffer->Release();
        vertexShaderBuffer = 0;

        pixelShaderBuffer->Release();
        pixelShaderBuffer = 0;

        // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
        D3D11_BUFFER_DESC matrixBufferDesc;
        matrixBufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
        matrixBufferDesc.ByteWidth           = sizeof(MatrixBufferType);
        matrixBufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
        matrixBufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
        matrixBufferDesc.MiscFlags           = 0;
        matrixBufferDesc.StructureByteStride = 0;

        // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
        result = device->CreateBuffer(&matrixBufferDesc, NULL, &mtxBuffer_);
        ENGINE_ASSERT_MSG(!FAILED(result),"Failed to create matrix buffer");
        load(device);
    }

    void Shader::Render( int indexCount,
                         D3DXMATRIX& worldMtx,
                         D3DXMATRIX& viewMtx,
                         D3DXMATRIX& projMtx )
    {
        ID3D11DeviceContext* deviceContext = API::Instance().DeviceContext();

        sendMatricies(deviceContext,worldMtx,viewMtx,projMtx,0);
        sendVertexShaderParams(deviceContext,1);
        sendPixelShaderParams(deviceContext,0);

        // Set the vertex input layout.
        deviceContext->IASetInputLayout(layout_);

        // Set the vertex and pixel shaders that will be used to render this triangle.
        deviceContext->VSSetShader(vertShader_, NULL, 0);
        deviceContext->PSSetShader(pixelShader_, NULL, 0);

        // Render the triangle.
        deviceContext->DrawIndexed(indexCount, 0, 0);
    }

    void Shader::Unload()
    {
        unload();
        // Release the matrix constant buffer.
        if(mtxBuffer_)
        {
            mtxBuffer_->Release();
            mtxBuffer_ = 0;
        }

        // Release the layout.
        if(layout_)
        {
            layout_->Release();
            layout_ = 0;
        }

        // Release the pixel shader.
        if(pixelShader_)
        {
            pixelShader_->Release();
            pixelShader_ = 0;
        }

        // Release the vertex shader.
        if(vertShader_)
        {
            vertShader_->Release();
            vertShader_ = 0;
        }
    }

    unsigned int Shader::Type() const
    {
        return type_;
    }

    //--------------------------------------------------------------------------

    bool Shader::loadShaderFile( const std::string& shaderProfile,
                                 const WCHAR* shaderFile,
                                 ID3D10Blob*& shaderBuffer,
                                 ID3D10Blob*& errorMsg)
    {
        // Compile the vertex shader code.
        HRESULT result = D3DX11CompileFromFile(shaderFile,
                                               NULL, 
                                               NULL, 
                                               "main",
                                               shaderProfile.c_str(),
                                               D3D10_SHADER_ENABLE_STRICTNESS,
                                               0,
                                               NULL, 
                                               &shaderBuffer,
                                               &errorMsg, NULL);
        if(FAILED(result))
            return false;
        return true;
    }

    void Shader::sendMatricies( ID3D11DeviceContext* deviceContext,
                                D3DXMATRIX& worldMtx,
                                D3DXMATRIX& viewMtx,
                                D3DXMATRIX& projMtx,
                                unsigned int bufferNumber)
    {
        HRESULT result;
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        MatrixBufferType* dataPtr;

        // Lock the constant buffer so it can be written to.
        result = deviceContext->Map(mtxBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        ENGINE_ASSERT_MSG(!FAILED(result), "Failed to lock the matrix buffer");

        // Get a pointer to the data in the constant buffer.
        dataPtr = (MatrixBufferType*)mappedResource.pData;

        dataPtr->normal = worldMtx;
        // Transpose the matrices to prepare them for the shader.
        D3DXMatrixTranspose(&worldMtx, &worldMtx);
        D3DXMatrixTranspose(&viewMtx, &viewMtx);
        D3DXMatrixTranspose(&projMtx, &projMtx);

        // Copy the matrices into the constant buffer.
        dataPtr->world      = worldMtx;
        dataPtr->view       = viewMtx;
        dataPtr->projection = projMtx;

        // Unlock the constant buffer.
        deviceContext->Unmap(mtxBuffer_, 0);

        // Finally set the constant buffer in the vertex shader with the updated values.
        deviceContext->VSSetConstantBuffers(bufferNumber, 1, &mtxBuffer_);
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

    void Shader::load(ID3D11Device* )
    {
    }

    void Shader::sendVertexShaderParams( ID3D11DeviceContext*&,
                                         unsigned int)
    {
    }

    void Shader::sendPixelShaderParams( ID3D11DeviceContext*&,
                                        unsigned int )
    {
    }

    void Shader::unload()
    {
    }
}

#endif