/******************************************************************************/
/*!
\file		AIEffectWander.cpp
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
#include "Simian/Interpolation.h"
#include "Simian/CModel.h"
#include "Simian/AnimationController.h"
#include "Simian/Math.h"

#include "EntityTypes.h"

#include "ComponentTypes.h"
#include "AIEffectWander.h"

#include "GCAttributes.h"
#include "GCPhysicsController.h"
#include "Pathfinder.h"
#include "TileMap.h"
#include "Simian/LogManager.h"

namespace Descension
{
    Simian::FactoryPlant<AIEffectBase, AIEffectWander> AIEffectWander::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AIEffectWander>(EFF_E_WANDER));

    static const Simian::f32 MAX_STEER_TIME = 0.2f;

    AIEffectWander::AIEffectWander()
        :   AIEffectBase(EFF_E_WANDER),
            transform_(0),
            physics_(0),
            model_(0),
            wander_(false),
            stateTrigger_(1),
            prevDir_(DIR_SOUTH),
            currentDir_(DIR_NORTH),
            minX_(0),
            minY_(0),
            maxX_(0),
            maxY_(0),
            wanderRadiusMin_(0.0f),
            wanderIntervalMin_(0.0f),
            idleIntervalMin_(0.0f),
            dirChangeIntervalMin_(0.0f),
            wanderRadiusMax_(1.0f),
            wanderIntervalMax_(2.0f),
            idleIntervalMax_(1.7f),
            dirChangeIntervalMax_(0.75f),
            steerTimer_(0.0f),
            wanderTimer_(0.0f),
            idleTimer_(0),
            dirChangeTimer_(0.0f),
            fsm_(WS_BIRTH,WS_TOTAL)
    {
        dirSequence_.reserve(DIR_TOTAL);
    }

    AIEffectWander::~AIEffectWander()
    {
    }
    //-Private------------------------------------------------------------------

    void AIEffectWander::onAwake_()
    {
    }

    void AIEffectWander::onInit_(GCAttributes* )
    {
        Target()->ComponentByType(Simian::C_TRANSFORM, transform_);
        Target()->ComponentByType(Descension::GC_PHYSICSCONTROLLER, physics_);
        Target()->ComponentByType(Simian::C_MODEL,model_);
        
        if(!transform_ || !physics_ || !model_)
            Enable(false);

        fsm_[WS_BIRTH].OnEnter   = Simian::Delegate::CreateDelegate<AIEffectWander, &AIEffectWander::birthOnEnter_>(this);
        fsm_[WS_BIRTH].OnUpdate  = Simian::Delegate::CreateDelegate<AIEffectWander, & AIEffectWander::birthOnUpdate_>(this);
        fsm_[WS_IDLE].OnEnter    = Simian::Delegate::CreateDelegate<AIEffectWander, &AIEffectWander::idleOnEnter_>(this);
        fsm_[WS_IDLE].OnUpdate   = Simian::Delegate::CreateDelegate<AIEffectWander, &AIEffectWander::idleOnUpdate_>(this);
        fsm_[WS_WANDER].OnEnter  = Simian::Delegate::CreateDelegate<AIEffectWander, &AIEffectWander::wanderOnEnter_>(this);
        fsm_[WS_WANDER].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectWander, &AIEffectWander::wanderOnUpdate_>(this);
    }

    void AIEffectWander::onUnload_()
    {
    }

    //-Public-------------------------------------------------------------------

    bool AIEffectWander::Update(Simian::f32 dt )
    {
        if(!Enable()) 
            return false;

        if(Target()->CurrentState() == stateTrigger_)
            fsm_.Update(dt);
        else if(wander_)
        {
            model_->Controller().PlayAnimation("Run");
            wander_ = false;
            fsm_.ChangeState(WS_BIRTH);
        }
        return false;
    }

    void AIEffectWander::serialize_( Simian::FileObjectSubNode& data )
    {
        data.AddData("RadiusMax",wanderRadiusMax_);
        data.AddData("RadiusMin",wanderRadiusMin_);
        data.AddData("WanderIntervalMax",wanderIntervalMax_);
        data.AddData("WanderIntervalMin",wanderIntervalMin_);
        data.AddData("IdleIntervalMax",idleIntervalMax_);
        data.AddData("IdleIntervalMin",idleIntervalMin_);
        data.AddData("DirChangeIntervalMax",dirChangeIntervalMax_);
        data.AddData("DirChangeIntervalMin",dirChangeIntervalMin_);
        data.AddData("StateTrigger",stateTrigger_);
    }

    void AIEffectWander::deserialize_( const Simian::FileObjectSubNode& data)
    {
        const Simian::FileDataSubNode* temp = data.Data("RadiusMax");
        wanderRadiusMax_ = temp? temp->AsF32() : wanderRadiusMax_;
        
        temp = data.Data("RadiusMin");
        wanderRadiusMin_ = temp? temp->AsF32() : wanderRadiusMin_;

        temp = data.Data("WanderIntervalMax");
        wanderIntervalMax_ = temp? temp->AsF32() : wanderIntervalMax_;
        temp = data.Data("WanderIntervalMin");
        wanderIntervalMin_ = temp? temp->AsF32() : wanderIntervalMin_;

        temp = data.Data("IdleIntervalMax");
        idleIntervalMax_ = temp? temp->AsF32() : idleIntervalMax_;
        temp = data.Data("IdleIntervalMin");
        idleIntervalMin_ = temp? temp->AsF32() : idleIntervalMin_;

        temp = data.Data("DirChangeIntervalMax");
        dirChangeIntervalMax_ = temp? temp->AsF32() : dirChangeIntervalMax_;
        temp = data.Data("DirChangeIntervalMin");
        dirChangeIntervalMin_ = temp? temp->AsF32() : dirChangeIntervalMin_;

        temp = data.Data("StateTrigger");
        stateTrigger_ = temp? temp->AsU32() : stateTrigger_;
    }

    //-Private------------------------------------------------------------------

    bool AIEffectWander::reachedTile_( Simian::f32 dt )
    {
        Simian::f32 futureDist = Target()->MovementSpeed() * dt;
        futureDist *= futureDist;
        Simian::Vector3 currentDist(nextTile_ - transform_->World().Position());
        return currentDist.LengthSquared() > futureDist;
    }

    bool AIEffectWander::updateNextTile_()
    {
        steerTimer_ = 0.0f;
        startDir_ = finalDir_;

        loadDirSequence_();
        Simian::Vector2 pos(transform_->World().Position().X(),transform_->World().Position().Z());
        pos = PathFinder::Instance().WorldToSubtile(pos);
        
        determineDir_(pos);

        pos = PathFinder::Instance().SubtileToWorld(pos);
        nextTile_.X(pos.X());
        nextTile_.Z(pos.Y());
        Simian::Vector3 dir = nextTile_ - transform_->World().Position();
        
        if(dir.LengthSquared() < Simian::Math::EPSILON)
            return false;
        else
        {
            Target()->MovementSpeed(Target()->MovementSpeedMax());
            finalDir_ = dir.Normalized();
        }

        startDir_ = Target()->Direction();
        return true;
    }

    void AIEffectWander::steer_( Simian::f32 dt )
    {
        steerTimer_ += dt;
        if(steerTimer_<= MAX_STEER_TIME)
        {
            Simian::f32 intp = steerTimer_/MAX_STEER_TIME;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            Target()->Direction(Simian::Interpolation::Interpolate<Simian::Vector3>(startDir_,finalDir_,intp,Simian::Interpolation::EaseCubicOut));
        }
        Simian::f32 angle = vectorToDegreeFloat_(Target()->Direction());
        transform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
    }

    void AIEffectWander::loadDirSequence_()
    {
        dirSequence_.clear();
        dirSequence_.push_back(currentDir_);
        for(Simian::u32 i = 0; i < DIR_TOTAL; ++i)
        {
            if( i == DIR_NORTH )
            {
                if( (prevDir_ != DIR_NORTH) && (currentDir_ != DIR_NORTH) )
                {
                    dirSequence_.push_back(DIR_NORTH);
                    continue;
                }
                ++i;
            }
            if( i == DIR_EAST )
            {
                if( (prevDir_ != DIR_EAST) && (currentDir_ != DIR_EAST) )
                {
                    dirSequence_.push_back(DIR_EAST);
                    continue;
                }
                ++i;
            }
            if( i == DIR_SOUTH )
            {
                if( (prevDir_ != DIR_SOUTH) && (currentDir_ != DIR_SOUTH) )
                {
                    dirSequence_.push_back(DIR_SOUTH);
                    continue;
                }
                ++i;
            }
            if( i == DIR_WEST )
            {
                if( (prevDir_ != DIR_WEST) && (currentDir_ != DIR_WEST) )
                {
                    dirSequence_.push_back(DIR_WEST);
                    continue;
                }
                ++i;
            }
        }
        dirSequence_.push_back(prevDir_);
    }

    void AIEffectWander::determineDir_( Simian::Vector2& subTilePos )
    {
        Simian::u32 minX = 0, minY = 0, maxX = 0, maxY = 0;
        PathFinder::Instance().CalcMinMax(physics_->Radius(),
            static_cast<Simian::u32>(subTilePos.X()),static_cast<Simian::u32>(subTilePos.Y()),minX,minY,maxX,maxY);

        Simian::u32 maxNumDir = dirSequence_.size();
        Simian::u32 i = 0;

        if(dirChangeTimer_ < dirChangeIntervalMin_ )
        {
            dirChangeTimer_ = Simian::Math::Random(dirChangeIntervalMin_,dirChangeIntervalMax_);
            i = Simian::Math::Random(0,dirSequence_.size() - 1);
        }

        bool validDir = false;
        std::vector<Simian::u32>::iterator it = dirSequence_.begin() + i;

        // Determine direction
        while( maxNumDir )
        {
            if(*it == DIR_NORTH)
            {
                Simian::u32 y = maxY + 1;
                if(!checkSpan_(y,minX, maxX, Tile::TILE_NORTH))
                {
                    prevDir_ = *it;
                    dirSequence_.erase(it);
                    i = (maxNumDir && --maxNumDir) ? Simian::Math::Random(0,maxNumDir) : 0;
                    it = dirSequence_.begin() + i;
                    continue;
                }
                else
                {
                    subTilePos.Y(subTilePos.Y()+1);
                    currentDir_ = DIR_NORTH;
                    validDir = true;
                    break;
                }                
            }
            if(*it == DIR_EAST)
            {
                Simian::u32 x = maxX + 1;
                if(!checkSpan_(x,minY, maxY, Tile::TILE_EAST) )
                {
                    prevDir_ = *it;
                    dirSequence_.erase(it);
                    i = (maxNumDir && --maxNumDir) ? Simian::Math::Random(0,maxNumDir) : 0;
                    it = dirSequence_.begin() + i;
                    continue;
                }
                else
                {
                    subTilePos.X(subTilePos.X()+1);
                    currentDir_ = DIR_EAST;
                    validDir = true;
                    break;
                }
            }
            if(*it == DIR_SOUTH )
            {
                Simian::u32 y = minY - 1;
                if(!checkSpan_(y,minX, maxX, Tile::TILE_SOUTH) )
                {
                    prevDir_ = *it;
                    dirSequence_.erase(it);
                    i = (maxNumDir && --maxNumDir) ? Simian::Math::Random(0,maxNumDir) : 0;
                    it = dirSequence_.begin() + i;
                    continue;
                }
                else
                {
                    subTilePos.Y(subTilePos.Y()-1);
                    currentDir_ = DIR_SOUTH;
                    validDir = true;
                    break;
                }
            }
            if(*it == DIR_WEST)
            {
                Simian::u32 x = minX - 1;
                if( !checkSpan_(x,minY, maxY, Tile::TILE_WEST) )    
                {
                    prevDir_ = *it;
                    dirSequence_.erase(it);
                    i = (maxNumDir && --maxNumDir) ? Simian::Math::Random(0,maxNumDir) : 0;
                    it = dirSequence_.begin() + i;
                    continue;
                }
                else
                {
                    subTilePos.X(subTilePos.X()-1);
                    currentDir_ = DIR_WEST;
                    validDir = true;
                    break;
                }
            }
        }
        if(!validDir)
        {
            subTilePos.X(static_cast<Simian::f32>(center_.X()));
            subTilePos.Y(static_cast<Simian::f32>(center_.Y()));
            //wanderTimer_ = 0.0f;
        }
    }

    Simian::f32 AIEffectWander::vectorToDegreeFloat_(const Simian::Vector3& vec)
    {
        // Get angle of rotation in degrees for vector
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

    bool AIEffectWander::isWithinBounds_( Simian::u32 x, Simian::u32 y )
    {
        return x >= minX_ && y >= minY_ && x < maxX_ && y < maxY_;
    }

    bool AIEffectWander::checkSpan_( Simian::u32 constAxis, Simian::u32 min, Simian::u32 max, Simian::u32 dir )
    {
        // Vertical
        if( (dir == Tile::TILE_NORTH) || (dir == Tile::TILE_SOUTH) )
        {
            while(min <= max)
            {
                if( !isWithinBounds_(min,constAxis) || !PathFinder::Instance().IsNextSubTileValid(dir,min,constAxis))
                    return false;
                ++min;
            }
            return true;
        }
        // Horizontal
        else
        {
            while(min <= max)
            {
                if( !isWithinBounds_(constAxis,min) || !PathFinder::Instance().IsNextSubTileValid(dir,constAxis,min))
                    return false;
                ++min;
            }
            return true;
        }            
    }

    void AIEffectWander::birthOnEnter_( Simian::DelegateParameter&  )
    {
        model_->Controller().PlayAnimation("Run");
    }

    void AIEffectWander::birthOnUpdate_( Simian::DelegateParameter&  )
    {
        Simian::Vector2 pos (transform_->World().Position().X(),transform_->World().Position().Z());
        pos = PathFinder::Instance().WorldToSubtile(pos);

        Simian::f32 actualRadius = 
            physics_->Radius() + Simian::Math::Random(wanderRadiusMin_,wanderRadiusMax_);

        PathFinder::Instance().CalcMinMax(actualRadius,
                                         static_cast<Simian::u32>(pos.X()),
                                         static_cast<Simian::u32>(pos.Y()),
                                         minX_,minY_,maxX_,maxY_);

        fsm_.ChangeState(WS_WANDER);
        
        center_.X(pos.X());
        center_.Y(pos.Y());
        wander_ = true;
    }

    void AIEffectWander::idleOnEnter_( Simian::DelegateParameter& )
    {
        idleTimer_ = Simian::Math::Random(idleIntervalMin_, idleIntervalMax_);
        model_->Controller().PlayAnimation("Idle");
        Target()->MovementSpeed(0);
        
        Simian::Vector2 pos (transform_->World().Position().X(),transform_->World().Position().Z());
        pos = PathFinder::Instance().WorldToSubtile(pos);

        Simian::f32 actualRadius = 
            physics_->Radius() + Simian::Math::Random(wanderRadiusMin_,wanderRadiusMax_);

        PathFinder::Instance().CalcMinMax(actualRadius,
            static_cast<Simian::u32>(pos.X()),
            static_cast<Simian::u32>(pos.Y()),
            minX_,minY_,maxX_,maxY_);

        fsm_.ChangeState(WS_WANDER);

        center_.X(pos.X());
        center_.Y(pos.Y());
    }

    void AIEffectWander::idleOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        idleTimer_ -= dt;

        if(idleTimer_ < idleIntervalMin_)
            fsm_.ChangeState(WS_WANDER);
    }

    void AIEffectWander::wanderOnEnter_( Simian::DelegateParameter&  )
    {
        wanderTimer_ = Simian::Math::Random(wanderIntervalMin_, wanderIntervalMax_);

        // wander timer is set to 0 if no valid dir found
        updateNextTile_();

        model_->Controller().PlayAnimation("Run");
        if( wanderTimer_ > 0 )
            Target()->MovementSpeed(Target()->MovementSpeedMax());
    }

    void AIEffectWander::wanderOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        wanderTimer_ -= dt;
        dirChangeTimer_ -= dt;

        if(wanderTimer_ < wanderIntervalMin_)
        {
            fsm_.ChangeState(WS_IDLE);
            return;
        }
        
        Simian::Vector2 pos(transform_->World().Position().X(),transform_->World().Position().Z());
        pos = PathFinder::Instance().WorldToSubtile(pos);

        Simian::u32 minX = 0, minY = 0, maxX = 0, maxY = 0;
        PathFinder::Instance().CalcMinMax(physics_->Radius(),
            static_cast<Simian::u32>(pos.X()),static_cast<Simian::u32>(pos.Y()),minX,minY,maxX,maxY);

        if(currentDir_ == DIR_NORTH)
        {
            Simian::u32 y = maxY + 1;
            if(!checkSpan_(y,minX, maxX, Tile::TILE_NORTH))
            {
                fsm_.ChangeState(WS_IDLE);
                return;
            }        
        }
        else if(currentDir_ == DIR_EAST)
        {
            Simian::u32 x = maxX + 1;
            if(!checkSpan_(x,minY, maxY, Tile::TILE_EAST) )
            {
                fsm_.ChangeState(WS_IDLE);
                return;
            }
        }
        else if(currentDir_ == DIR_SOUTH )
        {
            Simian::u32 y = minY - 1;
            if(!checkSpan_(y,minX, maxX, Tile::TILE_SOUTH) )
            {
                fsm_.ChangeState(WS_IDLE);
                return;
            }
        }
        else if(currentDir_ == DIR_WEST)
        {
            Simian::u32 x = minX - 1;
            if( !checkSpan_(x,minY, maxY, Tile::TILE_WEST) )    
            {
                fsm_.ChangeState(WS_IDLE);
                return;
            }
        }
    
        if(reachedTile_(dt))
            if(!updateNextTile_())
                fsm_.ChangeState(WS_IDLE);
        steer_(dt);
    }
}
