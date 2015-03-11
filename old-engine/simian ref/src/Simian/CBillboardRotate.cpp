/************************************************************************/
/*!
\file		CBillboardRotate.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CBillboardRotate.h"
#include "Simian/CCameraBase.h"
#include "Simian/CCamera.h"
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/Camera.h"
#include "Simian/CTransform.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CBillboardRotate> CBillboardRotate::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_BILLBOARDROTATE);

    CBillboardRotate::CBillboardRotate()
        : transform_(0),
          camera_(0),
          spin_(0)
    {
    }

    //--------------------------------------------------------------------------

    void CBillboardRotate::update_( DelegateParameter& )
    {
        // find the billboard matrix axis
        Simian::Vector3 z = camera_->Direction();
        Simian::Vector3 x = Simian::Vector3(-std::sin(spin_), std::cos(spin_)).Cross(z);
        x.Normalize();
        Simian::Vector3 y = z.Cross(x);
        y.Normalize();

        // build billboard matrix
        f32 yzzy = y.Z() - z.Y();
        f32 zxxz = z.X() - x.Z();
        f32 xyyx = x.Y() - y.X();
        f32 length = std::sqrt(yzzy * yzzy + zxxz * zxxz + xyyx * xyyx);
        
        // set the rotation
        Simian::Vector3 axis(yzzy/length, zxxz/length, xyyx/length);
        transform_->Rotation(axis, Simian::Radian(std::acos((x.X() + y.Y() + z.Z() - 1.0f)/2.0f)));
    }

    //--------------------------------------------------------------------------

    void CBillboardRotate::OnSharedLoad()
    {
        RegisterCallback(P_UPDATE, Delegate::CreateDelegate<CBillboardRotate, &CBillboardRotate::update_>(this));
    }

    void CBillboardRotate::OnAwake()
    {
        ComponentByType(C_TRANSFORM, transform_);
    }

    void CBillboardRotate::OnInit()
    {
        CCameraBase* sceneCamera;
        CCameraBase::SceneCamera(sceneCamera);
        camera_ = &sceneCamera->Camera();
    }

    void CBillboardRotate::Serialize( FileObjectSubNode& data )
    {
        data.AddData("Spin", Simian::Radian(spin_).Degrees());
    }

    void CBillboardRotate::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("Spin", spin_, Simian::Radian(spin_).Degrees());
        spin_ = Simian::Degree(spin_).Radians();
    }
}
