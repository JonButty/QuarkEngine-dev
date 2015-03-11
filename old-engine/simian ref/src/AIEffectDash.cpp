/******************************************************************************/
/*!
\file		AIEffectDash.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/Scene.h"
#include "Simian/CTransform.h"
#include "Simian/Math.h"
#include "Simian/Interpolation.h"
#include "Simian/DebugRenderer.h"
#include "Simian/CModel.h"
#include "Simian/AnimationController.h"
#include "Simian/EngineComponents.h"

#include "AIEffectDash.h"

#include "GCPhysicsController.h"
#include "GCAttributes.h"
#include "GCPlayerLogic.h"
#include "PathFinder.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"
#include "EntityTypes.h"
#include "TileWallCollision.h"
#include "ComponentTypes.h"

namespace Descension
{
    Simian::FactoryPlant<AIEffectBase, AIEffectDash> AIEffectDash::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AIEffectDash>(EFF_E_DASH));

    AIEffectDash::AIEffectDash()
        :   AIEffectBase(EFF_E_DASH),
            map_(0),
            model_(0),
            sourceTransform_(0),
            targetTransform_(0),
            dash_(false),
            attacked_(false),
            animationFPS_(0),
            timer_(0),
            accelTimer_(0),
            deaccelTimer_(0),
            startDelay_(0.25f),
            endDelay_(0.35f),
            totalTime_(1.0f),
            dashTime_(0),
            combinedRadius_(0),
            dashStartDist_(2.0f),
            dashStopDist_(0.1f),
            fsm_(DS_START, DS_TOTAL)
    {
    }

    AIEffectDash::~AIEffectDash()
    {
    }

    //-Private------------------------------------------------------------------

    void AIEffectDash::onAwake_()
    {
    }

    void AIEffectDash::onInit_( GCAttributes* source )
    {
        totalTime_ = dashTime_ + startDelay_ + endDelay_;
        GCPhysicsController* sourcePhysics = 0;
        if(source)
        {
            source->ComponentByType(Simian::C_TRANSFORM,sourceTransform_);
            source->ComponentByType(GC_PHYSICSCONTROLLER,sourcePhysics);
        }
        // Default
        else
        {
            GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(GC_PHYSICSCONTROLLER,sourcePhysics);
            GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,sourceTransform_);
        }

        GCPhysicsController* targetPhysics = 0;
        Target()->ParentScene()->EntityByType(E_TILEMAP)->ComponentByType(GC_TILEMAP,map_);
        Target()->ComponentByType(Simian::C_TRANSFORM,targetTransform_);
        Target()->ComponentByType(GC_PHYSICSCONTROLLER,targetPhysics);
        Target()->ComponentByType(Simian::C_MODEL,model_);
        
        if(!map_ || !targetPhysics || !sourcePhysics || !targetTransform_ || !sourceTransform_ || !model_)
            Enable(false);
        else
        {
            targetRadius_ = targetPhysics->Radius();
            combinedRadius_ = targetRadius_ + sourcePhysics->Radius();
            scaledTargetRadius_ = targetRadius_ * TileWallCollision::PHYSICS_SCALE;
            attackRadius_ = Target()->AttackRange() + combinedRadius_;
        }
        fsm_[DS_START].OnEnter = Simian::Delegate::CreateDelegate<AIEffectDash, &AIEffectDash::startOnEnter_>(this);
        fsm_[DS_START].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectDash, &AIEffectDash::startOnUpdate_>(this);
        fsm_[DS_DASH].OnEnter = Simian::Delegate::CreateDelegate<AIEffectDash, &AIEffectDash::dashOnEnter_>(this);
        fsm_[DS_DASH].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectDash, &AIEffectDash::dashOnUpdate_>(this);
        fsm_[DS_END].OnEnter = Simian::Delegate::CreateDelegate<AIEffectDash, &AIEffectDash::endOnEnter_>(this);
        fsm_[DS_END].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectDash, &AIEffectDash::endOnUpdate_>(this);
    }

    void AIEffectDash::onUnload_()
    {
    }

    //-Public-------------------------------------------------------------------

    bool AIEffectDash::Update( Simian::f32 dt )
    {
        if(!Enable() || GCPlayerLogic::Instance()->Attributes()->Health() < Simian::Math::EPSILON)
            return false;
        
        destination_ = sourceTransform_->World().Position();
        vecToPlayer_ = destination_ - targetTransform_->World().Position();
        Simian::Vector3 pos = targetTransform_->World().Position()*TileWallCollision::PHYSICS_SCALE;
        Simian::f32 distToPlayer = std::abs(vecToPlayer_.Length());

        if(!dash_ && distToPlayer < dashStartDist_ && distToPlayer > dashStopDist_ &&
           !TileWallCollision::CheckTileCollision(pos,vecToPlayer_*TileWallCollision::PHYSICS_SCALE,
           scaledTargetRadius_, &map_->TileMap(),false) && checkPath_())
        {
            dash_ = true;
            timer_ = 0.0f;
            accelTimer_ = 0.0f;
        }

        if(dash_ && timer_ < totalTime_ )
        {
            timer_ += dt;
            fsm_.Update(dt);
            if(timer_ > totalTime_)
                model_->Controller().PlayAnimation("Run");
            
            return true;
        }
        else if(dash_ && ((distToPlayer >= dashStartDist_)))
        {
            Target()->MovementSpeed(Target()->MovementSpeedMax());
            fsm_.ChangeState(DS_START);
            dash_ = false;
            attacked_ = false;
            timer_ = 0.0f;
        }
        return false;
    }

    void AIEffectDash::serialize_( Simian::FileObjectSubNode& node )
    {
        node.AddData("StartDelay",startDelay_);
        node.AddData("EndDelay",endDelay_);
        node.AddData("DashTime",dashTime_);
        node.AddData("StartDist",dashStartDist_);
        node.AddData("StopDist",dashStopDist_);
        node.AddData("AnimationFrameRate",animationFPS_);
    }

    void AIEffectDash::deserialize_( const Simian::FileObjectSubNode& node )
    {
        const Simian::FileDataSubNode* data = node.Data("StartDelay");
        startDelay_ = data? data->AsF32() : startDelay_;

        data = node.Data("EndDelay");
        endDelay_ = data? data->AsF32() : endDelay_;

        data = node.Data("DashTime");
        dashTime_ = data? data->AsF32() : dashTime_;

        data = node.Data("StartDist");
        dashStartDist_ = data? data->AsF32() : dashStartDist_;

        data = node.Data("StopDist");
        dashStopDist_ = data? data->AsF32() : dashStopDist_;

        data = node.Data("AnimationFrameRate");
        animationFPS_ = data? data->AsU32() : animationFPS_;
    }

    Simian::f32 AIEffectDash::vectorToDegreeFloat_(const Simian::Vector3& vec)
    {
        // Get angle of rotation in degrees for vector
        //float x = Simian::Math::Clamp(vec.X(), -1.0f, 1.0f);
        if(vec.LengthSquared() > Simian::Math::EPSILON)
        {
            Simian::Vector3 tempVec = vec.Normalized();
            Simian::f32 angle = std::acosf(tempVec.X());
            if (tempVec.Z() > 0)
                angle = Simian::Math::TAU - angle;
            // Wrap angle within 360
            angle = Simian::Math::WrapAngle(Simian::Radian(angle)).Degrees();

            return angle;
        }
        return 0;
    }

    void AIEffectDash::startOnEnter_( Simian::DelegateParameter& )
    {
        speed_ = (vecToPlayer_.Length() - combinedRadius_ - dashStopDist_)/dashTime_;
        Simian::Vector3 normalized(vecToPlayer_.Normalized());
        model_->Controller().PlayAnimation("Dash", false, animationFPS_);
        Target()->MovementSpeed(0);
        Target()->Direction( normalized );
        Simian::f32 angle = vectorToDegreeFloat_( normalized );
        targetTransform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
    }

    void AIEffectDash::startOnUpdate_( Simian::DelegateParameter&  )
    {
        if( timer_ > startDelay_ )
            fsm_.ChangeState(DS_DASH);
    }

    void AIEffectDash::dashOnEnter_( Simian::DelegateParameter& )
    {
        accelTimer_ = 0.0f;
    }

    void AIEffectDash::dashOnUpdate_( Simian::DelegateParameter& param)
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        accelTimer_ += dt;
        
        if(accelTimer_ < dashTime_)
        {
            Simian::f32 intp = accelTimer_/dashTime_;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            Target()->MovementSpeed(Simian::Interpolation::Interpolate<Simian::f32>(
                0.0f,speed_,intp,Simian::Interpolation::EaseCubicOut));
        }
        if(!attacked_)
        {
            // Get distance vector to player
            Simian::f32 eToPLength = vecToPlayer_.Length()-combinedRadius_;
            // Check if distance to enemy is within attack range
            if (eToPLength <= attackRadius_)
            {
                Simian::f32 angle = targetTransform_->Angle().Radians();
                // Check if facing enemy
                Simian::f32 dotProd = vecToPlayer_.Dot(
                    Simian::Vector3(std::cos(angle-Simian::Math::PI/2), 0,
                    std::sin(angle+Simian::Math::PI/2)));

                if ((dotProd/eToPLength) < Target()->AttackAngle())
                {
                    CombatCommandSystem::Instance().AddCommand(Target(), GCPlayerLogic::Instance()->Attributes(),
                        CombatCommandDealDamage::Create(Target()->Damage()));
                    attacked_ = true;
                }
            }
        }
        if( accelTimer_ > dashTime_ )
            fsm_.ChangeState(DS_END);
    }

    void AIEffectDash::endOnEnter_( Simian::DelegateParameter&  )
    {
        deaccelTimer_ = 0.0f;
    }

    void AIEffectDash::endOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        deaccelTimer_ += dt;

        if(deaccelTimer_ < endDelay_)
        {
            Simian::f32 intp = deaccelTimer_/endDelay_;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            Target()->MovementSpeed(Simian::Interpolation::Interpolate<Simian::f32>(
                Target()->MovementSpeed(),0,intp,Simian::Interpolation::EaseCubicOut));
        }
    }

    bool AIEffectDash::checkPath_()
    {
        // Destination bounding box
        PathNode dest(static_cast<Simian::u32>(destination_.X()),
            static_cast<Simian::u32>(destination_.Z()));
        Simian::u32 destMinX = 0, destMinY = 0, destMaxX = 0, destMaxY = 0;
        PathFinder::Instance().CalcMinMax(combinedRadius_ - targetRadius_, dest.X(),dest.Y(), destMinX, destMinY, destMaxX, destMaxY);

        // Current bounding box
        PathNode node(static_cast<Simian::u32>(targetTransform_->World().Position().X()),
            static_cast<Simian::u32>(targetTransform_->World().Position().Z()));

        Simian::u32 minX = 0, minY = 0, maxX = 0, maxY = 0;
        PathFinder::Instance().CalcMinMax(targetRadius_, node.X(),node.Y(), minX, minY, maxX, maxY);

        Simian::u32 step = 2 * static_cast<Simian::u32>(PathFinder::Instance().WorldToSubtile(targetRadius_));

        Simian::u32 dx = step + static_cast<Simian::u32>((destination_.X() - targetTransform_->World().Position().X()));
        Simian::u32 dy = step + static_cast<Simian::u32>((destination_.Z() - targetTransform_->World().Position().Z()));

        Simian::u32 maxSteps = static_cast<Simian::u32>(Simian::Math::Max(vecToPlayer_.X()/targetRadius_,vecToPlayer_.Z()/targetRadius_));
        
        // Use SAT to determine when bounding boxes intersect
        for(Simian::u32 i = 0; i < maxSteps; ++i)
        {
            for( Simian::u32 x = minX; x <= maxX; ++x )
            {
                for( Simian::u32 y = minY; y < maxY; ++y)
                {
                    if(!PathFinder::Instance().IsSubTileValid(x,y))
                        return false;
                    PathFinder::Instance().DrawSubtile(x,y,Simian::Color(1.0f,0.0f,0.0f));
                }
            }
            minX += dx;
            maxX += dx;
            minY += dy;
            minY += dy;
            if((minX > destMaxX || maxX < destMinX) && (minY > destMaxY || maxY < destMinY))
                return false;
        }
        return true;
    }

}
