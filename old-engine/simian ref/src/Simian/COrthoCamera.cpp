/************************************************************************/
/*!
\file		COrthoCamera.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/COrthoCamera.h"
#include "Simian/EngineComponents.h"
#include "Simian/EnginePhases.h"
#include "Simian/Camera.h"
#include "Simian/Matrix.h"
#include "Simian/CTransform.h"
#include "Simian/GraphicsDevice.h"

namespace Simian
{
    FactoryPlant<EntityComponent, COrthoCamera> COrthoCamera::factoryPlant_ =
        FactoryPlant<EntityComponent, COrthoCamera>(
        &GameFactory::Instance().ComponentFactory(), C_ORTHOCAMERA);

    COrthoCamera::COrthoCamera()
        : near_(0.0f),
          far_(10000.0f)
    {
    }

    //--------------------------------------------------------------------------

    const Simian::Vector2& COrthoCamera::Size() const
    {
        return size_;
    }

    void COrthoCamera::Size( const Simian::Vector2& val )
    {
        size_ = val;
    }

    Simian::f32 COrthoCamera::Near() const
    {
        return near_;
    }

    void COrthoCamera::Near( Simian::f32 val )
    {
        near_ = val;
    }

    Simian::f32 COrthoCamera::Far() const
    {
        return far_;
    }

    void COrthoCamera::Far( Simian::f32 val )
    {
        far_ = val;
    }

    //--------------------------------------------------------------------------

    void COrthoCamera::updatePosition_( DelegateParameter& )
    {
        // if its a ui camera keep refreshing its size
        if (uiCamera_)
            size_ = Device()->Size();

        camera_->View(transform_->World().Inversed());
        camera_->Projection(Matrix::Ortho(size_.X(), size_.Y(), near_, far_));
    }

    //--------------------------------------------------------------------------

    void COrthoCamera::OnSharedLoad()
    {
        CCameraBase::OnSharedLoad();
        RegisterCallback(P_UPDATE, Delegate::CreateDelegate<COrthoCamera, 
            &COrthoCamera::updatePosition_>(this));
    }

    void COrthoCamera::OnAwake()
    {
        CCameraBase::OnAwake();
        camera_->Projection(Matrix::Ortho(size_.X(), size_.Y(), near_, far_));

        if (uiCamera_)
        {
            size_ = Device()->Size();
            transform_->Position(Simian::Vector3(0, 0, -far_ * 0.5f));
        }
    }

    void COrthoCamera::Serialize( FileObjectSubNode& data )
    {
        CCameraBase::Serialize(data);

        FileObjectSubNode* sizeNode = data.AddObject("Size");
        sizeNode->AddData("Width", size_.X());
        sizeNode->AddData("Height", size_.Y());

        data.AddData("Near", near_);
        data.AddData("Far", far_);
    }

    void COrthoCamera::Deserialize( const FileObjectSubNode& data )
    {
        CCameraBase::Deserialize(data);

        const FileObjectSubNode* sizeNode = data.Object("Size");
        if (sizeNode)
        {
            const FileDataSubNode* width = sizeNode->Data("Width");
            const FileDataSubNode* height = sizeNode->Data("Height");
            size_ = Simian::Vector2(
                width ? width->AsF32() : size_.X(),
                height ? height->AsF32() : size_.Y());
        }

        const FileDataSubNode* nearNode = data.Data("Near");
        near_ = nearNode ? nearNode->AsF32() : near_;

        const FileDataSubNode* farNode = data.Data("Far");
        far_ = farNode ? farNode->AsF32() : far_;
    }
}
