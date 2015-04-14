/******************************************************************************/
/*!
\file		AIEffectFleeLocation.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/11/01
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian\Delegate.h"
#include "Simian\EngineComponents.h"
#include "Simian\EntityComponent.h"
#include "Simian\Scene.h"
#include "Simian\CModel.h"
#include "Simian\AnimationController.h"
#include "Simian\Math.h"
#include "Simian\CTransform.h"
#include "Simian\Interpolation.h"

#include "Pathfinder.h"

#include "ComponentTypes.h"
#include "EntityTypes.h"

#include "GCPlayerLogic.h"
#include "GCAttributes.h"
#include "GCPhysicsController.h"

#include "AIEffectFleeLocation.h"
#include "Simian\LogManager.h"

static const Simian::f32 DIR_MIDDLE = std::sqrt(2.0f)/2.0f;
static const Simian::f32 DIR_STEP_DEGREES = 5.0f;
static const Simian::f32 RETRY_MAX_TIME = 25.0f;
static const Simian::u32 NUM_DIR_STEPS = static_cast<Simian::u32>(360.0f/DIR_STEP_DEGREES);
static const Simian::u32 MAX_COLL_COUNT = 1;
static const Simian::f32 MAX_ANGLE_OFFSET = 360.0f;

namespace Descension
{
    Simian::FactoryPlant<AIEffectBase, AIEffectFleeLocation> AIEffectFleeLocation::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AIEffectFleeLocation>(EFF_E_FLEE_LOCATION));

    AIEffectFleeLocation::AIEffectFleeLocation()
        :   AIEffectBase(EFF_E_FLEE_LOCATION),
            flee_(false),
            playerTrans_(0),
            tarTrans_(0),
            tarPhys_(0),
            model_(0),
            animationSpeed_(24),
            currDistSqr_(0),
            timer_(0),
            fleeTime_(10),
            fleeRadius_(2),
            fleeRadiusSqr_(4),
            fleeSpeed_(1),
            steerTimer_(0),
            maxSteerTime_(0.7f),
            startDir_(Simian::Vector3::Zero),
            finalDir_(Simian::Vector3::Zero),
            nextTile_(Simian::Vector3::Zero),
            fleeLocation_(Simian::Vector3::Zero),
            fsm_(FS_PREFLEE,FS_TOTAL)
    {
    }

    AIEffectFleeLocation::~AIEffectFleeLocation()
    {
    }

    bool AIEffectFleeLocation::Update( Simian::f32 dt )
    {
        if(!Enable()|| GCPlayerLogic::Instance()->Attributes()->Health() < Simian::Math::EPSILON)
            return false;
        currDistSqr_ = (playerTrans_->WorldPosition() - tarTrans_->WorldPosition()).LengthSquared();
        
        if(currDistSqr_ < fleeRadiusSqr_)
            flee_ = true;
        if(flee_)
        {
            fsm_.Update(dt);
            return flee_;
        }
        return false;
    }

    //--------------------------------------------------------------------------

    void AIEffectFleeLocation::onAwake_()
    {
    }

    void AIEffectFleeLocation::onInit_( GCAttributes* )
    {
        GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,playerTrans_);

        Target()->ComponentByType(Simian::C_TRANSFORM, tarTrans_);
        Target()->ComponentByType(Descension::GC_PHYSICSCONTROLLER, tarPhys_);
        Target()->ComponentByType(Simian::C_MODEL,model_);

        if(!playerTrans_ || !tarTrans_ || !tarPhys_ || !model_)
            Enable(false);
        else
            radius_ = tarPhys_->Radius();
        fsm_[FS_PREFLEE].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectFleeLocation, &AIEffectFleeLocation::preFleeOnUpdate_>(this);
        fsm_[FS_FLEE].OnEnter = Simian::Delegate::CreateDelegate<AIEffectFleeLocation, &AIEffectFleeLocation::fleeOnEnter_>(this);
        fsm_[FS_FLEE].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectFleeLocation, &AIEffectFleeLocation::fleeOnUpdate_>(this);
    }

    void AIEffectFleeLocation::onUnload_()
    {
    }

    void AIEffectFleeLocation::serialize_( Simian::FileObjectSubNode& node )
    {
        node;
    }

    void AIEffectFleeLocation::deserialize_( const Simian::FileObjectSubNode& node )
    {
       node;
    }

    void AIEffectFleeLocation::preFleeOnUpdate_( Simian::DelegateParameter&  )
    {
        model_->Controller().PlayAnimation("Run",true,animationSpeed_);
        Target()->MovementSpeed(Target()->MovementSpeedMax());
        fsm_.ChangeState(FS_FLEE);
    }

    void AIEffectFleeLocation::fleeOnEnter_( Simian::DelegateParameter&  )
    {
       determineFleeLocation_();
       timer_ = fleeTime_;
    }

    void AIEffectFleeLocation::fleeOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        timer_ -= dt;

        if(timer_ < 0)
        {
            fsm_.ChangeState(FS_PREFLEE);
            flee_ = false;
            return;
        }

        updateNextTile_();
        steer_(dt);

        Simian::f32 distToEnd((fleeLocation_ - tarTrans_->WorldPosition()).LengthSquared());
        if(distToEnd < Simian::Math::EPSILON)
        {
            fsm_.ChangeState(FS_PREFLEE);
            flee_ = false;
            return;
        }
    }

    void AIEffectFleeLocation::steer_( Simian::f32 dt )
    {
        steerTimer_ += dt;

        if(steerTimer_<= maxSteerTime_)
        {
            Simian::f32 intp = steerTimer_/maxSteerTime_;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            Target()->Direction(Simian::Interpolation::Interpolate<Simian::Vector3>(startDir_,finalDir_,intp,Simian::Interpolation::EaseCubicOut));
        }
        
        Simian::f32 angle = vectorToDegreeFloat_(Target()->Direction());
        tarTrans_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
    }

    Simian::f32 AIEffectFleeLocation::vectorToDegreeFloat_(const Simian::Vector3& vec)
    {
        // Get angle of rotation in degrees for vector
        if(vec.LengthSquared() > Simian::Math::EPSILON)
        {
            Simian::Vector3 tempVec = vec.Normalized();
            Simian::f32 angle = std::acos(tempVec.X());
            if (tempVec.Z() > 0)
                angle = Simian::Math::TAU - angle;
            // Wrap angle within 360
            angle = Simian::Math::WrapAngle(Simian::Radian(angle)).Degrees();

            return angle;
        }
        return 0;
    }

    void AIEffectFleeLocation::determineFleeLocation_()
    {
        Simian::Vector2 currPos(tarTrans_->WorldPosition().X(),tarTrans_->WorldPosition().Z());
        Simian::Vector2 fleeDir(playerTrans_->WorldPosition().X() - currPos.X(), playerTrans_->WorldPosition().Z() - currPos.Y());
        fleeDir.Normalize();
        Simian::Vector2 fleeVec(fleeDir.X() * fleeRadius_,fleeDir.Y()*fleeRadius_);
        Simian::f32 angleOffset = 0.0f;
        Simian::f32 startAngle = std::asin(fleeVec.Y()/fleeRadius_);
        Simian::f32 finalAngle = startAngle;

        while(angleOffset < 360.0f)
        {
            Simian::Vector2 newPos(PathFinder::Instance().WorldToSubtile(currPos + fleeVec));
            Simian::u32 uNewPosX = static_cast<Simian::u32>(newPos.X());
            Simian::u32 uNewPosY = static_cast<Simian::u32>(newPos.Y());
            if(PathFinder::Instance().IsSubTileValid(uNewPosX,uNewPosY))
            {
                fleeLocation_.X(newPos.X());
                fleeLocation_.Z(newPos.Y());
                break;
            }
            else
            {
                Simian::f32 intp = angleOffset/MAX_ANGLE_OFFSET;
                intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
                angleOffset = Simian::Interpolation::Interpolate
                    <Simian::f32>(startAngle,MAX_ANGLE_OFFSET,intp,Simian::Interpolation::Linear);
                finalAngle += angleOffset;
                fleeVec.X(fleeRadius_ * std::cos(finalAngle));
                fleeVec.Y(fleeRadius_ * std::sin(finalAngle));
            }
        }
    }

    void AIEffectFleeLocation::updateNextTile_()
    {
        nextTile_ = PathFinder::Instance().GeneratePath(radius_, fleeRadiusSqr_, 
            tarTrans_->WorldPosition(),fleeLocation_);
        finalDir_ = nextTile_ - tarTrans_->WorldPosition();
        
        steerTimer_ = 0.0f;
        startDir_ = Target()->Direction();
    }
}

