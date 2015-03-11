#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_11

#include "shaderPhongDX11.h"
#include "shaderTypes.h"
#include "cameraManager.h"
#include "camera.h"
#include "lightAmbient.h"

namespace GFX
{
    Util::FactoryPlant<Shader,ShaderPhong> ShaderPhong::factoryPlant_(ShaderFactory::InstancePtr(),ST_PHONG);

    ShaderPhong::ShaderPhong()
        :   Shader(ST_PHONG),
            cameraBuffer_(0),
            lightBuffer_(0)
    {
    }

    ShaderPhong::ShaderPhong( const ShaderPhong& val)
        :   Shader(val)
    {
    }

    //--------------------------------------------------------------------------

    void ShaderPhong::load(ID3D11Device* device)
    {
        cameraBuffer_ = 0;
        lightBuffer_ = 0;

        D3D11_BUFFER_DESC cameraBufferDesc;
        D3D11_BUFFER_DESC lightBufferDesc;

        // Setup the description of the camera dynamic constant buffer that is in the vertex shader.
        cameraBufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
        cameraBufferDesc.ByteWidth           = sizeof(CameraBufferType);
        cameraBufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
        cameraBufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
        cameraBufferDesc.MiscFlags           = 0;
        cameraBufferDesc.StructureByteStride = 0;

        // Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
        HRESULT result = device->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer_);
        ENGINE_ASSERT_MSG(!FAILED(result),"Failed to create camera buffer");

        // Setup the description of the light dynamic constant buffer that is in the pixel shader.
        // Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
        lightBufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
        lightBufferDesc.ByteWidth           = sizeof(LightBufferType);
        lightBufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
        lightBufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
        lightBufferDesc.MiscFlags           = 0;
        lightBufferDesc.StructureByteStride = 0;

        // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
        result = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer_);
        ENGINE_ASSERT_MSG(!FAILED(result),"Failed to create light buffer");

        light_ = LightManager::Instance().GetLights(LT_POINT)[0];
    }

    void ShaderPhong::sendVertexShaderParams( ID3D11DeviceContext*& deviceContext,
                                              unsigned int startingBufferNumber)
    {
        CameraBufferType* camBufferPtr;
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        // Lock the camera constant buffer so it can be written to.
        HRESULT result = deviceContext->Map(cameraBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        ENGINE_ASSERT_MSG(!FAILED(result),"Failed to lock the camera constant buffer");

        // Get a pointer to the data in the constant buffer.
        camBufferPtr = reinterpret_cast<CameraBufferType*>(mappedResource.pData);

        // Copy the camera position into the constant buffer.
        Camera* camera = CameraManager::Instance().GetCamera(CameraManager::Instance().GetCurrentCamera());
        camBufferPtr->cameraPosition.x = -camera->Pos().X();
        camBufferPtr->cameraPosition.y = -camera->Pos().Y();
        camBufferPtr->cameraPosition.z = -camera->Pos().Z();

        camBufferPtr->padding = 0.0f;

        // Unlock the camera constant buffer.
        deviceContext->Unmap(cameraBuffer_, 0);

        // Now set the camera constant buffer in the vertex shader with the updated values.
        deviceContext->VSSetConstantBuffers(startingBufferNumber, 1, &cameraBuffer_);
    }

    void ShaderPhong::sendPixelShaderParams( ID3D11DeviceContext*& deviceContext,
                                             unsigned int startingBufferNumber )
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        LightBufferType* lightBufferPtr;

        // Lock the light constant buffer so it can be written to.
        HRESULT result = deviceContext->Map(lightBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        ENGINE_ASSERT_MSG(!FAILED(result),"Failed to lock the light constant buffer");

        // Get a pointer to the data in the light constant buffer.
        lightBufferPtr = reinterpret_cast<LightBufferType*>(mappedResource.pData);

        LightPoint* ambientLight = reinterpret_cast<LightPoint*>(LightManager::Instance().GetLight(light_));

        // Copy the lighting variables into the light constant buffer.
        lightBufferPtr->ambientColor.x  = ambientLight->Ambient().R();
        lightBufferPtr->ambientColor.y  = ambientLight->Ambient().B();
        lightBufferPtr->ambientColor.z  = ambientLight->Ambient().G();
        lightBufferPtr->ambientColor.w  = ambientLight->Ambient().A();

        lightBufferPtr->diffuseColor.x  = ambientLight->Diffuse().R();
        lightBufferPtr->diffuseColor.y  = ambientLight->Diffuse().B();
        lightBufferPtr->diffuseColor.z  = ambientLight->Diffuse().G();
        lightBufferPtr->diffuseColor.w  = ambientLight->Diffuse().A();
        
        lightBufferPtr->specularColor.x = ambientLight->Specular().R();
        lightBufferPtr->specularColor.y = ambientLight->Specular().B();
        lightBufferPtr->specularColor.z = ambientLight->Specular().G();
        lightBufferPtr->specularColor.w = ambientLight->Specular().A();

        lightBufferPtr->lightPos.x = -ambientLight->Pos().X();
        lightBufferPtr->lightPos.y = -ambientLight->Pos().Y();
        lightBufferPtr->lightPos.z = -ambientLight->Pos().Z();

        lightBufferPtr->specularPower = ambientLight->SpecularPower();

        // Unlock the light constant buffer.
        deviceContext->Unmap(lightBuffer_, 0);

        // Finally set the light constant buffer in the pixel shader with the updated values.
        deviceContext->PSSetConstantBuffers(startingBufferNumber, 1, &lightBuffer_);
    }

    void ShaderPhong::unload()
    {
        if(cameraBuffer_)
        {
            cameraBuffer_->Release();
            cameraBuffer_ = 0;
        }

        if(lightBuffer_)
        {
            lightBuffer_->Release();
            lightBuffer_ = 0;
        }
    }
}
#endif