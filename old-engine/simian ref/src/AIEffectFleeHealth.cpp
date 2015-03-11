/******************************************************************************/
/*!
\file		AIEffectFleeHealth.cpp
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

#include "AIEffectFleeHealth.h"
#include "Simian\LogManager.h"
#include "Simian\DebugRenderer.h"

static const Simian::f32 DIR_MIDDLE = std::sqrt(2.0f)/2.0f;
static const Simian::f32 DIR_STEP_DEGREES = 2.0f;
static const Simian::u32 NUM_DIR_STEPS = static_cast<Simian::u32>(360.0f/DIR_STEP_DEGREES);
static const Simian::f32 MAX_ANGLE_OFFSET = 360.0f;

namespace Descension
{
    Simian::FactoryPlant<AIEffectBase, AIEffectFleeHealth> AIEffectFleeHealth::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AIEffectFleeHealth>(EFF_E_FLEE_HEALTH));

    AIEffectFleeHealth::AIEffectFleeHealth()
        :   AIEffectBase(EFF_E_FLEE_HEALTH),
            stop_(false),
            playerTrans_(0),
            tarTrans_(0),
            model_(0),
            animationSpeed_(0),
            minHealth_(0),
            timer_(0),
            fleeTime_(10),
            fleeRadius_(0),
            fleeRadiusSqr_(0),
            fleeSpeed_(0),
            steerTimer_(0),
            maxSteerTime_(0),
            startDir_(Simian::Vector3::Zero),
            finalDir_(Simian::Vector3::Zero),
            nextTile_(Simian::Vector3::Zero),
            fleeLocation_(Simian::Vector3::Zero),
            fsm_(FS_PREFLEE,FS_TOTAL)
    {
    }

    AIEffectFleeHealth::~AIEffectFleeHealth()
    {
    }

    bool AIEffectFleeHealth::Update( Simian::f32 dt )
    {
        if(!Enable() || GCPlayerLogic::Instance()->Attributes()->Health() < Simian::Math::EPSILON)
            return false;
        if(!stop_ && Target()->Health() < minHealth_)
        {
            fsm_.Update(dt);
            return !stop_;
        }
        return false;
    }

    //--------------------------------------------------------------------------
    
    void AIEffectFleeHealth::onAwake_()
    {
    }

    void AIEffectFleeHealth::onInit_( GCAttributes* )
    {
        GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,playerTrans_);

        GCPhysicsController* tarphys_ = 0;
        Target()->ComponentByType(Simian::C_TRANSFORM, tarTrans_);
        Target()->ComponentByType(GC_PHYSICSCONTROLLER, tarphys_);
        Target()->ComponentByType(Simian::C_MODEL,model_);

        if(!playerTrans_ || !tarTrans_ || !tarphys_ || !model_)
            Enable(false);
        else
            radius_ = tarphys_->Radius();
        fsm_[FS_PREFLEE].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectFleeHealth, &AIEffectFleeHealth::preFleeOnUpdate_>(this);
        fsm_[FS_FLEE].OnEnter = Simian::Delegate::CreateDelegate<AIEffectFleeHealth, &AIEffectFleeHealth::fleeOnEnter_>(this);
        fsm_[FS_FLEE].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectFleeHealth, &AIEffectFleeHealth::fleeOnUpdate_>(this);
    }

    void AIEffectFleeHealth::onUnload_()
    {
    }

    void AIEffectFleeHealth::serialize_( Simian::FileObjectSubNode& node )
    {
        node.AddData("AnimationSpeed",animationSpeed_);
        node.AddData("MinHealth",minHealth_);
        node.AddData("MaxSteerTime", maxSteerTime_);
        node.AddData("FleeTimeMax", fleeTime_);
        node.AddData("FleeSpeed", fleeSpeed_);
        node.AddData("FleeRadius", fleeRadius_);
    }

    void AIEffectFleeHealth::deserialize_( const Simian::FileObjectSubNode& node )
    {
        node.Data("AnimationSpeed",animationSpeed_,animationSpeed_);
        node.Data("MinHealth",minHealth_,minHealth_);
        node.Data("MaxSteerTime", maxSteerTime_,maxSteerTime_);
        node.Data("FleeTimeMax", fleeTime_,fleeTime_);
        node.Data("FleeSpeed", fleeSpeed_,fleeSpeed_);
        node.Data("FleeRadius", fleeRadius_,fleeRadius_);
        fleeRadiusSqr_ = fleeRadius_ * fleeRadius_;
    }

    void AIEffectFleeHealth::preFleeOnUpdate_( Simian::DelegateParameter&  )
    {
        model_->Controller().PlayAnimation("Run",true,animationSpeed_);
        Target()->MovementSpeed(fleeSpeed_);
        fsm_.ChangeState(FS_FLEE);
    }

    void AIEffectFleeHealth::fleeOnEnter_( Simian::DelegateParameter&  )
    {
        determineFleeLocation_();
        timer_ = fleeTime_;
    }

    void AIEffectFleeHealth::fleeOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        timer_ -= dt;

        if(timer_ < 0)
        {
            fsm_.ChangeState(FS_PREFLEE);
            stop_ = true;
            return;
        }

        if(reachedNextTile_(dt))
            updateNextTile_();
        steer_(dt);

        Simian::f32 distToEnd((fleeLocation_ - tarTrans_->WorldPosition()).LengthSquared());
        if(distToEnd < 0.1f)
        {
            model_->Controller().PlayAnimation("Idle");
            fsm_.ChangeState(FS_PREFLEE);
            stop_ = true;
        }
    }

    void AIEffectFleeHealth::steer_( Simian::f32 dt )
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

    Simian::f32 AIEffectFleeHealth::vectorToDegreeFloat_(const Simian::Vector3& vec)
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

    void AIEffectFleeHealth::determineFleeLocation_()
    {
        Simian::Vector2 currPos(tarTrans_->WorldPosition().X(),tarTrans_->WorldPosition().Z());
        Simian::Vector3 playerPos(playerTrans_->WorldPosition());
        Simian::Vector2 fleeDir(currPos.X() - playerPos.X(), currPos.Y() - playerPos.Z());
        fleeDir.Normalize();
        Simian::Vector2 fleeVec(fleeDir.X() * fleeRadius_,fleeDir.Y()*fleeRadius_);
        Simian::f32 angleOffset = 0.0f;
        Simian::f32 startAngle = std::asin(fleeVec.Y()/fleeRadius_);
        Simian::f32 finalAngle = startAngle;
        fleeLocation_ = tarTrans_->WorldPosition();

        while(angleOffset < 360.0f)
        {
            Simian::Vector2 newPos(PathFinder::Instance().WorldToSubtile(currPos + fleeVec));
            Simian::u32 uNewPosX = static_cast<Simian::u32>(newPos.X());
            Simian::u32 uNewPosY = static_cast<Simian::u32>(newPos.Y());
            if(PathFinder::Instance().IsSubTileValid(uNewPosX,uNewPosY))
            {
                newPos = PathFinder::Instance().SubtileToWorld(newPos);
                fleeLocation_.X(newPos.X());
                fleeLocation_.Z(newPos.Y());
                updateNextTile_();
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

    void AIEffectFleeHealth::updateNextTile_()
    {
        nextTile_ = PathFinder::Instance().GeneratePath(radius_, fleeRadiusSqr_, 
            tarTrans_->WorldPosition(),fleeLocation_);
        finalDir_ = nextTile_ - tarTrans_->WorldPosition();

        steerTimer_ = 0.0f;
        startDir_ = Target()->Direction();
    }

    bool AIEffectFleeHealth::reachedNextTile_( Simian::f32 dt )
    {
        Simian::f32 futureDist = Target()->MovementSpeed() * dt;
        futureDist *= futureDist;
        Simian::Vector3 currentDist(nextTile_ - tarTrans_->WorldPosition());
        Simian::f32 dist = currentDist.LengthSquared();
        return ((dist < Simian::Math::EPSILON) || (dist > futureDist));
    }
}
