/*************************************************************************/
/*!
\file		GCDescensionCamera.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCDescensionCamera.h"
#include "ComponentTypes.h"
#include "GCPlayerLogic.h"

#include "Simian\EnginePhases.h"
#include "Simian\CTransform.h"
#include "Simian\Interpolation.h"
#include "Simian\Math.h"
#include "Simian\LogManager.h"

namespace Descension
{
    GCDescensionCamera* GCDescensionCamera::instance_ = 0;
    Simian::FactoryPlant<Simian::EntityComponent, GCDescensionCamera> GCDescensionCamera::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_DESCENSIONCAMERA);

    GCDescensionCamera::GCDescensionCamera()
        : interpolating_(false),
          interpolator_(Simian::Interpolation::Linear),
          shakeDuration_(0.0f),
          shakeAmount_(0.0f)
    {
    }

    //--------------------------------------------------------------------------

    GCDescensionCamera* GCDescensionCamera::Instance()
    {
        return instance_;
    }

    bool GCDescensionCamera::Interpolating() const
    {
        return interpolating_;
    }

    void GCDescensionCamera::Interpolating( bool val )
    {
        interpolating_ = val;
    }

    const Simian::Vector3& GCDescensionCamera::FinalPosition() const
    {
        return finalPosition_;
    }

    void GCDescensionCamera::FinalPosition( const Simian::Vector3& val )
    {
        finalPosition_ = val;
    }

    const Simian::Vector3& GCDescensionCamera::FinalYawPitchZoom() const
    {
        return finalYawPitchZoom_;
    }

    void GCDescensionCamera::FinalYawPitchZoom( const Simian::Vector3& val )
    {
        finalYawPitchZoom_ = val;
    }

    Simian::Vector3 GCDescensionCamera::YawPitchZoom() const
    {
        return Simian::Vector3(
            yawTransform_->Angle().Degrees(),
            pitchTransform_->Angle().Degrees(),
            cameraTransform_->Position().Z());
    }

    const Simian::Vector3& GCDescensionCamera::StartingYawPitchZoom() const
    {
        return startingYawPitchZoom_;
    }

    //--------------------------------------------------------------------------

    void GCDescensionCamera::update_( Simian::DelegateParameter& param )
    {
        Simian::EntityUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;
        if (interpolating_)
        {
            Simian::f32 intp = timer_/duration_;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

            // interpolate camera
            Simian::Vector3 pos = Simian::Interpolation::Interpolate(initialPosition_, finalPosition_, intp, interpolator_);
            Simian::Vector3 yawPitchZoom = Simian::Interpolation::Interpolate(initialYawPitchZoom_, finalYawPitchZoom_, intp, interpolator_);

            if (baseTransform_->ParentTransform())
                baseTransform_->ParentTransform()->World().Inversed().Transform(pos);
            baseTransform_->Position(pos);
            yawTransform_->Rotation(yawTransform_->Axis(), Simian::Degree(yawPitchZoom.X()));
            pitchTransform_->Rotation(pitchTransform_->Axis(), Simian::Degree(yawPitchZoom.Y()));
            cameraTransform_->Position(Simian::Vector3(cameraTransform_->Position().X(), cameraTransform_->Position().Y(), yawPitchZoom.Z()));

            // set interpolating to false
            if (intp >= 1.0f)
                interpolating_ = false;
        }

        if (shakeDuration_ > 0.0f && p->Dt > 0.0f)
        {
            shakeDuration_ -= p->Dt;

            Simian::f32 shakeX = Simian::Math::Random(-shakeAmount_, shakeAmount_);
            Simian::f32 shakeY = Simian::Math::Random(-shakeAmount_, shakeAmount_);

            cameraTransform_->Position(Simian::Vector3(shakeX, shakeY, cameraTransform_->Position().Z()));
        }
        else
            cameraTransform_->Position(Simian::Vector3(0.0f, 0.0f, cameraTransform_->Position().Z()));
    }

    //--------------------------------------------------------------------------

    void GCDescensionCamera::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCDescensionCamera, &GCDescensionCamera::update_>(this));
    }

    void GCDescensionCamera::OnAwake()
    {
        instance_ = this;

        ComponentByType(Simian::C_TRANSFORM, baseTransform_);

        Simian::Entity* entity = ParentEntity()->ChildByName("Camera Yaw");
        entity->ComponentByType(Simian::C_TRANSFORM, yawTransform_);

        entity = entity->ChildByName("Camera Pitch");
        entity->ComponentByType(Simian::C_TRANSFORM, pitchTransform_);

        entity = entity->ChildByName("Camera");
        entity->ComponentByType(Simian::C_TRANSFORM, cameraTransform_);
        entity->ComponentByType(Simian::C_CAMERA, camera_);

        startingYawPitchZoom_ = YawPitchZoom();
    }

    void GCDescensionCamera::Interpolate( const Simian::Vector3& destination, const Simian::Vector3& yawPitchZoom, Simian::f32 time, Simian::f32(*interpolator)(Simian::f32) )
    {
        // force unlock the camera
        GCPlayerLogic::Instance()->CameraLocked(false);

        initialPosition_ = baseTransform_->World().Position();
        initialYawPitchZoom_ = Simian::Vector3(
            yawTransform_->Angle().Degrees(),
            pitchTransform_->Angle().Degrees(),
            cameraTransform_->Position().Z());

        interpolating_ = true;
        finalPosition_ = destination;
        finalYawPitchZoom_ = initialYawPitchZoom_ + yawPitchZoom;
        interpolator_ = interpolator;
        duration_ = time;
        timer_ = 0.0f;
    }

    void GCDescensionCamera::Shake( Simian::f32 amount, Simian::f32 duration )
    {
        shakeAmount_ = amount;
        shakeDuration_ = duration;
    }

    void GCDescensionCamera::Serialize( Simian::FileObjectSubNode& )
    {
    }

    void GCDescensionCamera::Deserialize( const Simian::FileObjectSubNode& )
    {
    }
}
