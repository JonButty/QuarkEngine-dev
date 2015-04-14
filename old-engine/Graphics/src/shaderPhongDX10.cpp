#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10

#include "shaderPhongDX10.h"
#include "lightManager.h"
#include "lightTypes.h"
#include "cameraDX.h"
#include "cameraManager.h"
#include "lightPoint.h"

namespace GFX
{
    Util::FactoryPlant<Shader,ShaderPhong> ShaderPhong::factoryPlant_(ShaderFactory::InstancePtr(),ST_PHONG);

    ShaderPhong::ShaderPhong()
        :   Shader(ST_PHONG),
            cameraPosPtr_(0),
            ambientColorPtr_(0),
            diffuseColorPtr_(0),
            lightPosPtr_(0),
            specularColorPtr_(0),
            specularPowerPtr_(0)
    {
    }

    ShaderPhong::ShaderPhong( const ShaderPhong& val )
        :   Shader(val),
            cameraPosPtr_(val.cameraPosPtr_),
            ambientColorPtr_(val.ambientColorPtr_),
            diffuseColorPtr_(val.diffuseColorPtr_),
            lightPosPtr_(val.lightPosPtr_),
            specularColorPtr_(val.specularColorPtr_),
            specularPowerPtr_(val.specularPowerPtr_)
    {
    }

    void ShaderPhong::load(ID3D10Effect* effect)
    {
        light_            = LightManager::Instance().GetLights(LT_POINT)[0];
        cameraPosPtr_     = effect->GetVariableByName("CameraPos")->AsVector();
        ambientColorPtr_  = effect->GetVariableByName("AmbientColor")->AsVector();
        diffuseColorPtr_  = effect->GetVariableByName("DiffuseColor")->AsVector();
        lightPosPtr_      = effect->GetVariableByName("LightPos")->AsVector();
        specularPowerPtr_ = effect->GetVariableByName("SpecularPower")->AsScalar();
        specularColorPtr_ = effect->GetVariableByName("SpecularColor")->AsVector();
    }

    void ShaderPhong::sendVertexShaderParams()
    {
        Camera* camera = CameraManager::Instance().GetCamera(CameraManager::Instance().GetCurrentCamera());
        Math::Vec3F pos(camera->Pos());
        float vec4[4] = {pos.X(), pos.Y(), pos.Z(), 1.0f};

        cameraPosPtr_->SetFloatVector(vec4);
    }

    void ShaderPhong::sendPixelShaderParams()
    {
        LightPoint* ambientLight = reinterpret_cast<LightPoint*>(LightManager::Instance().GetLight(light_));
        
        Math::Vec3F pos(ambientLight->Pos());
        float vec4[4] = {pos.X(), pos.Y(), pos.Z(), 1.0f};
        lightPosPtr_->SetFloatVector(vec4);

        Color color(ambientLight->Ambient());
        vec4[0] = color.R();
        vec4[1] = color.G();
        vec4[2] = color.B();
        vec4[3] = color.A();
        ambientColorPtr_->SetFloatVector(vec4);

        color = ambientLight->Diffuse();
        vec4[0] = color.R();
        vec4[1] = color.G();
        vec4[2] = color.B();
        vec4[3] = color.A();
        diffuseColorPtr_->SetFloatVector(vec4);

        color = ambientLight->Specular();
        vec4[0] = color.R();
        vec4[1] = color.G();
        vec4[2] = color.B();
        vec4[3] = color.A();
        specularColorPtr_->SetFloatVector(vec4);

        float scalar = ambientLight->SpecularPower();
        specularPowerPtr_->SetFloat(scalar);
    }
}

#endif