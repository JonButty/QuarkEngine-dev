/************************************************************************/
/*!
\file		CCamera.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CCamera.h"
#include "Simian/EngineComponents.h"
#include "Simian/Camera.h"
#include "Simian/CTransform.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/EnginePhases.h"
#include "Simian/RenderQueue.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CCamera> CCamera::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_CAMERA);

    CCamera::CCamera()
        : fovy_(Simian::Degree(60.0f)),
          near_(1.0f),
          far_(10000.0f)
    {
    }

    //--------------------------------------------------------------------------

    const Simian::Angle& CCamera::Fovy() const
    {
        return fovy_;
    }

    void CCamera::Fovy( const Simian::Angle& val )
    {
        fovy_ = Simian::Radian(val.Radians());
    }

    Simian::f32 CCamera::Near() const
    {
        return near_;
    }

    void CCamera::Near( Simian::f32 val )
    {
        near_ = val;
    }

    Simian::f32 CCamera::Far() const
    {
        return far_;
    }

    void CCamera::Far( Simian::f32 val )
    {
        far_ = val;
    }

    //--------------------------------------------------------------------------

    void CCamera::updatePosition_( DelegateParameter& )
    {
        camera_->View(transform_->World().Inversed());
        camera_->Projection(Matrix::PerspectiveFOV(fovy_, device_->Size().X()/device_->Size().Y(), near_, far_));
    }

    //--------------------------------------------------------------------------

    void CCamera::OnSharedLoad()
    {
        CCameraBase::OnSharedLoad();
        RegisterCallback(P_UPDATE, Delegate::CreateDelegate<CCamera, &CCamera::updatePosition_>(this));
    }

    void CCamera::OnAwake()
    {
        CCameraBase::OnAwake();

        camera_->Projection(Matrix::PerspectiveFOV(fovy_, device_->Size().X()/device_->Size().Y(), near_, far_));

        // compute distance from screen
        if (uiCamera_)
            transform_->Position(Simian::Vector3(0, 0, -(device_->Size().Y() * 0.5f) / std::tan(fovy_.Radians() * 0.5f)));
    }

    void CCamera::Serialize( FileObjectSubNode& data )
    {
        CCameraBase::Serialize(data);

        data.AddData("FovY", fovy_.Degrees());
        data.AddData("Near", near_);
        data.AddData("Far", far_);
    }

    void CCamera::Deserialize( const FileObjectSubNode& data )
    {
        CCameraBase::Deserialize(data);

        const FileDataSubNode* fovy = data.Data("FovY");
        fovy_ = fovy ? Radian(Degree(fovy->AsF32()).Radians()) : fovy_;

        const FileDataSubNode* nearNode = data.Data("Near");
        near_ = nearNode ? nearNode->AsF32() : near_;

        const FileDataSubNode* farNode = data.Data("Far");
        far_ = farNode ? farNode->AsF32() : far_;
    }
}
