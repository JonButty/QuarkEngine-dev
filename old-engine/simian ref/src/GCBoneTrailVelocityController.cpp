/******************************************************************************/
/*!
\file		GCBoneTrailVelocityController.cpp
\author 	Bryan Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "GCBoneTrailVelocityController.h"
#include "ComponentTypes.h"

#include "Simian\EnginePhases.h"
#include "Simian\EngineComponents.h"
#include "Simian\LogManager.h"
#include "Simian\CBoneTrail.h"
#include "Simian\Math.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCBoneTrailVelocityController> GCBoneTrailVelocityController::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_BONETRAILVELOCITYCONTROLLER);

    GCBoneTrailVelocityController::GCBoneTrailVelocityController()
        : trail_(0),
          minVelocity_(0.0f),
          maxVelocity_(1.0f),
          enabled_(true)
    {
    }
    
    //--------------------------------------------------------------------------

    void GCBoneTrailVelocityController::Enabled(bool enabled)
    {
        enabled_ = enabled;
    }

    bool GCBoneTrailVelocityController::Enabled() const
    {
        return enabled_;
    }

    void GCBoneTrailVelocityController::MinVelocity(Simian::f32 minVelocity)
    {
        minVelocity_ = minVelocity;
    }

    Simian::f32 GCBoneTrailVelocityController::MinVelocity() const
    {
        return minVelocity_;
    }
        
    void GCBoneTrailVelocityController::MaxVelocity(Simian::f32 maxVelocity)
    {
        maxVelocity_ = maxVelocity;
    }

    Simian::f32 GCBoneTrailVelocityController::MaxVelocity() const
    {
        return maxVelocity_;
    }

    //--------------------------------------------------------------------------

    void GCBoneTrailVelocityController::update_( Simian::DelegateParameter& param )
    {
        Simian::EntityUpdateParameter* p;
        param.As(p);

        // find the difference in positions
        Simian::Vector3 curPos = currentPosition_();
        Simian::Vector3 dist = curPos - lastPosition_;
        float vel = dist.Length()/p->Dt;

        // map velocity to opacity
        float opacity = (vel - minVelocity_)/(maxVelocity_ - minVelocity_);
        opacity = Simian::Math::Clamp(opacity, 0.0f, 1.0f);
        trail_->Opacity(enabled_ ? opacity : 0.0f);

        lastPosition_ = curPos;
    }

    Simian::Vector3 GCBoneTrailVelocityController::currentPosition_() const
    {
        return 0.5f * (trail_->TopJointPosition() + trail_->BottomJointPosition());
    }

    //--------------------------------------------------------------------------

    void GCBoneTrailVelocityController::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCBoneTrailVelocityController, &GCBoneTrailVelocityController::update_>(this));
    }

    void GCBoneTrailVelocityController::OnAwake()
    {
        // get bone trail component
        ComponentByType(Simian::C_BONETRAIL, trail_);

        if (!trail_)
        {
            SWarn("GCBoneTrailVelocityController requires a bone trail.");
            return;
        }
    }

    void GCBoneTrailVelocityController::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("MinVelocity", minVelocity_);
        data.AddData("MaxVelocity", maxVelocity_);
        data.AddData("Enabled", enabled_);
    }

    void GCBoneTrailVelocityController::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("MinVelocity", minVelocity_, minVelocity_);
        data.Data("MaxVelocity", maxVelocity_, minVelocity_);
        data.Data("Enabled", enabled_, enabled_);
    }
}
