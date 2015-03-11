/******************************************************************************/
/*!
\file		GCCinematicMotion.cpp
\author 	Bryan Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "GCCinematicMotion.h"
#include "ComponentTypes.h"

#include "Simian/EnginePhases.h"
#include "Simian/CTransform.h"
#include "Simian/CUIResize.h"
#include "Simian/Math.h"
#include "Simian/Interpolation.h"
#include "Simian/LogManager.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCCinematicMotion> GCCinematicMotion::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_CINEMATICMOTION);

    GCCinematicMotion::GCCinematicMotion()
        : initialAngle_(0.0f),
          finalAngle_(0.0f),
          angleTimer_(0.0f),
          angleDuration_(0.0f),
          movementTimer_(0.0f),
          movementDuration_(0.0f),
          enabled_(true),
          transform_(0)
    {
    }

    //--------------------------------------------------------------------------

    void GCCinematicMotion::update_( Simian::DelegateParameter& param )
    {
        // skip if disabled
        if (!enabled_)
            return;

        Simian::EntityUpdateParameter* p;
        param.As(p);

        if (angleTimer_ < angleDuration_)
        {
            angleTimer_ += p->Dt;
            Simian::f32 intp = angleTimer_/angleDuration_;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            Simian::f32 angle = Simian::Interpolation::Interpolate(initialAngle_, finalAngle_, intp, Simian::Interpolation::Linear);
            transform_->Rotation(transform_->Axis(), Simian::Radian(angle));
        }

        if (movementTimer_ < movementDuration_)
        {
            movementTimer_ += p->Dt;
            Simian::f32 intp = movementTimer_/movementDuration_;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            Simian::Vector3 position = Simian::Interpolation::Interpolate(initialPosition_, finalPosition_, intp, Simian::Interpolation::Linear);
            (this->*setPosition_)(position);
        }
    }

    Simian::Vector3 GCCinematicMotion::uiPosition_()
    {
        return ui_->Position();
    }

    void GCCinematicMotion::uiPosition_( const Simian::Vector3& pos )
    {
        ui_->Position(pos);
    }

    Simian::Vector3 GCCinematicMotion::transformPosition_()
    {
        return transform_->WorldPosition();
    }

    void GCCinematicMotion::transformPosition_( const Simian::Vector3& pos )
    {
        transform_->WorldPosition(pos);
    }

    //--------------------------------------------------------------------------

    void GCCinematicMotion::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCCinematicMotion, &GCCinematicMotion::update_>(this));
    }

    void GCCinematicMotion::OnAwake()
    {
        // get the transform.. that's all we need
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_UIRESIZE, ui_);

        setPosition_ = &GCCinematicMotion::transformPosition_;
        getPosition_ = &GCCinematicMotion::transformPosition_;

        if (ui_)
        {
            setPosition_ = &GCCinematicMotion::uiPosition_;
            getPosition_ = &GCCinematicMotion::uiPosition_;
        }
    }

    void GCCinematicMotion::Rotate( const Simian::Angle& target, Simian::f32 duration )
    {
        // wrap the angles
        Simian::Degree initialAngle = Simian::Math::WrapAngle(transform_->Angle());
        Simian::Degree targetAngle = Simian::Math::WrapAngle(target);

        // find the shortest increment
        Simian::f32 d = Simian::Math::NearestAngleDifference(initialAngle, targetAngle).Radians();

        // compute initial and final
        initialAngle_ = initialAngle.Radians();
        finalAngle_ = targetAngle.Radians() + d;

        // set timer and duration
        angleTimer_ = 0.0f;
        angleDuration_ = duration;
    }

    void GCCinematicMotion::Translate( const Simian::Vector3& target, Simian::f32 duration )
    {
        initialPosition_ = (this->*getPosition_)();
        finalPosition_ = target;
        movementTimer_ = 0.0f;
        movementDuration_ = duration;
    }

    void GCCinematicMotion::Serialize( Simian::FileObjectSubNode& data)
    {
        data.AddData("Enabled", enabled_);
    }

    void GCCinematicMotion::Deserialize( const Simian::FileObjectSubNode& data)
    {
        data.Data("Enabled", enabled_, enabled_);
    }
}
