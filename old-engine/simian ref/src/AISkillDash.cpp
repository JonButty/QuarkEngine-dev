/*************************************************************************/
/*!
\file		AISkillDash.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/Scene.h"
#include "Simian/CTransform.h"
#include "Simian/Math.h"
#include "Simian/Interpolation.h"
#include "Simian/DebugRenderer.h"
#include "Simian/CModel.h"
#include "Simian/AnimationController.h"
#include "Simian/EngineComponents.h"
#include "Simian/DebugRenderer.h"
#include "Simian/CSoundEmitter.h"

#include "AISkillDash.h"

#include "GCPhysicsController.h"
#include "GCAttributes.h"
#include "GCPlayerLogic.h"
#include "PathFinder.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"
#include "EntityTypes.h"
#include "TileWallCollision.h"
#include "ComponentTypes.h"
#include "GCEditor.h"

namespace Descension
{
    Simian::FactoryPlant<AIEffectBase, AISkillDash> AISkillDash::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AISkillDash>(EFF_S_DASH));

    AISkillDash::AISkillDash()
        :   AISkillBase(EFF_S_DASH),
            map_(0),
            model_(0),
            sourceTransform_(0),
            targetTransform_(0),
            targetPhysics_(0),
            sounds_(0),
            dash_(false),
            attacked_(false),
            animationFPS_(0),
            dashSoundNum_(3),
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
            damage_(0),
            fsm_(DS_START, DS_TOTAL)
    {
    }

    AISkillDash::~AISkillDash()
    {

    }

    //--------------------------------------------------------------------------

    bool AISkillDash::Update( Simian::f32 dt )
    {
        if(!Enable())
            return false;

        DebugRendererDrawCircle(targetTransform_->World().Position(),dashStartDist_,
            Simian::Color(0.0f, 1.0f, 0.0f));
        destination_ = sourceTransform_->World().Position();
        vecToPlayer_ = destination_ - targetTransform_->World().Position();
        Simian::Vector3 pos = targetTransform_->World().Position()*TileWallCollision::PHYSICS_SCALE;
        Simian::f32 distToPlayer = vecToPlayer_.LengthSquared();

        if(!dash_ && distToPlayer < dashStartDistSqr_ && distToPlayer > dashStopDistSqr_ &&
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
        else if(dash_ )//&& ((distToPlayer >= dashStartDistSqr_) || timer_ > totalTime_))
        {
            Target()->MovementSpeed(Target()->MovementSpeedMax());
            fsm_.ChangeState(DS_START);
            dash_ = false;
            attacked_ = false;
            timer_ = 0.0f;
        }
        return false;
    }

    //--------------------------------------------------------------------------

    void AISkillDash::onAwake_()
    {
    }

    void AISkillDash::onInit_( GCAttributes* source )
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

        Target()->ParentScene()->EntityByType(E_TILEMAP)->ComponentByType(GC_TILEMAP,map_);
        Target()->ComponentByType(Simian::C_TRANSFORM,targetTransform_);
        Target()->ComponentByType(GC_PHYSICSCONTROLLER,targetPhysics_);
        Target()->ComponentByType(Simian::C_MODEL,model_);
        Target()->ComponentByType(Simian::C_SOUNDEMITTER,sounds_);

        if(!map_ || !targetPhysics_ || !sounds_ || !sourcePhysics || !targetTransform_ || !sourceTransform_ || !model_)
            Enable(false);
        else
        {
            targetRadius_ = targetPhysics_->Radius();
            combinedRadius_ = targetRadius_ + sourcePhysics->Radius();
            scaledTargetRadius_ = targetRadius_ * TileWallCollision::PHYSICS_SCALE;
            attackRadius_ = Target()->AttackRange() + combinedRadius_;
        }
        fsm_[DS_START].OnEnter = Simian::Delegate::CreateDelegate<AISkillDash, &AISkillDash::startOnEnter_>(this);
        fsm_[DS_START].OnUpdate = Simian::Delegate::CreateDelegate<AISkillDash, &AISkillDash::startOnUpdate_>(this);
        fsm_[DS_DASH].OnEnter = Simian::Delegate::CreateDelegate<AISkillDash, &AISkillDash::dashOnEnter_>(this);
        fsm_[DS_DASH].OnUpdate = Simian::Delegate::CreateDelegate<AISkillDash, &AISkillDash::dashOnUpdate_>(this);
        fsm_[DS_END].OnEnter = Simian::Delegate::CreateDelegate<AISkillDash, &AISkillDash::endOnEnter_>(this);
        fsm_[DS_END].OnUpdate = Simian::Delegate::CreateDelegate<AISkillDash, &AISkillDash::endOnUpdate_>(this);
    }

    void AISkillDash::onUnload_()
    {

    }

    void AISkillDash::serialize_( Simian::FileObjectSubNode& node )
    {
        node.AddData("StartDelay",startDelay_);
        node.AddData("EndDelay",endDelay_);
        node.AddData("DashTime",dashTime_);
        node.AddData("StartDist",dashStartDist_);
        node.AddData("StopDist",dashStopDist_);
        node.AddData("AnimationFrameRate",animationFPS_);
        node.AddData("Damage",damage_);
        node.AddData("DashSoundNum",dashSoundNum_);
    }

    void AISkillDash::deserialize_( const Simian::FileObjectSubNode& node )
    {
        const Simian::FileDataSubNode* data = node.Data("StartDelay");
        startDelay_ = data? data->AsF32() : startDelay_;

        data = node.Data("EndDelay");
        endDelay_ = data? data->AsF32() : endDelay_;

        data = node.Data("DashTime");
        dashTime_ = data? data->AsF32() : dashTime_;

        data = node.Data("StartDist");
        dashStartDist_ = data? data->AsF32() : dashStartDist_;
        dashStartDistSqr_ = dashStartDist_ * dashStartDist_;

        data = node.Data("StopDist");
        dashStopDist_ = data? data->AsF32() : dashStopDist_;
        dashStopDistSqr_ = dashStopDist_ * dashStopDist_;

        data = node.Data("AnimationFrameRate");
        animationFPS_ = data? data->AsU32() : animationFPS_;

        data = node.Data("Damage");
        damage_ = data? static_cast<Simian::s32>(data->AsF32()) : damage_;

        data = node.Data("DashSoundNum");
        dashSoundNum_ = data? static_cast<Simian::u32>(data->AsF32()) : dashSoundNum_;
    }

    void AISkillDash::startOnEnter_( Simian::DelegateParameter&  )
    {
        speed_ = (vecToPlayer_.Length() - combinedRadius_ - dashStopDist_)/dashTime_;
        Simian::Vector3 normalized(vecToPlayer_.Normalized());
        model_->Controller().PlayAnimation("Dash Raise", false, animationFPS_);
        Target()->MovementSpeed(0);
        Target()->Direction( normalized );
        Simian::f32 angle = vectorToDegreeFloat_( normalized );
        targetTransform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
    }

    void AISkillDash::startOnUpdate_( Simian::DelegateParameter& )
    {
        if( timer_ > startDelay_ )
            fsm_.ChangeState(DS_DASH);
    }

    void AISkillDash::dashOnEnter_( Simian::DelegateParameter& )
    {
        accelTimer_ = 0.0f;
        model_->Controller().PlayAnimation("Dash Down", false);
        if (sounds_)
            sounds_->Play(dashSoundNum_);
    }

    void AISkillDash::dashOnUpdate_( Simian::DelegateParameter& param )
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
                        CombatCommandDealDamage::Create(damage_));
                    attacked_ = true;
                }
            }
        }
        if( accelTimer_ > dashTime_ )
            fsm_.ChangeState(DS_END);
    }

    void AISkillDash::endOnEnter_( Simian::DelegateParameter&  )
    {
        deaccelTimer_ = 0.0f;
    }

    void AISkillDash::endOnUpdate_( Simian::DelegateParameter& param )
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

    Simian::f32 AISkillDash::vectorToDegreeFloat_( const Simian::Vector3& vec )
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

    bool AISkillDash::checkPath_()
    {
        // Destination bounding box
        Simian::f32 sourceRadius = combinedRadius_ - targetRadius_;
        sourceRadius = PathFinder::Instance().WorldToSubtile(sourceRadius);
        Simian::Vector3 normVecToPlayer(vecToPlayer_.Normalized());
        Simian::Vector2 destVec( PathFinder::Instance().WorldToSubtile(Simian::Vector2(destination_.X(), destination_.Z())));
        destVec.X(destVec.X() - 4 * sourceRadius * normVecToPlayer.X());
        destVec.Y(destVec.Y() - 4 * sourceRadius * normVecToPlayer.Z());
        PathNode dest(static_cast<Simian::u32>(destVec.X()),static_cast<Simian::u32>(destVec.Y()));

        Simian::u32 minX = 0, minY = 0, maxX = 0, maxY = 0;
        PathFinder::Instance().CalcMinMax(combinedRadius_ - targetRadius_, dest.X(),dest.Y(), minX, minY, maxX, maxY);

        for(Simian::u32 x = minX; x <= maxX; ++x)
        {
            for(Simian::u32 y = minY; y <= maxY; ++y)
            {
                if(!PathFinder::Instance().IsSubTileValid(x,y))
                {
                    PathFinder::Instance().DrawSubtile(x,y,Simian::Color(1.0f,0.0f,0.0f));
                    return false;
                }
                PathFinder::Instance().DrawSubtile(x,y,Simian::Color(0.0f,0.0f,1.0f));
            }
        }
        return true;
    }
}
