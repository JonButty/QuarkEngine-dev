/******************************************************************************/
/*!
\file		AIEffectCreep.cpp
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

#include "AIEffectCreep.h"

#include "TileWallCollision.h"
#include "EntityTypes.h"

#include "GCAIGeneric.h"
#include "GCPhysicsController.h"
#include "GCAttributes.h"
#include "ComponentTypes.h"

namespace Descension
{
    Simian::FactoryPlant<AIEffectBase, AIEffectCreep> AIEffectCreep::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AIEffectCreep>(EFF_E_CREEP));

    AIEffectCreep::AIEffectCreep()
        :   AIEffectBase(EFF_E_CREEP),
            map_(0),
            sourceTransform_(0),
            targetTransform_(0),
            speedRatio_(0),
            creepRadiusSqr_(0),
            stopRadiusSqr_(0),
            creepStartDist_(0),
            creepEndDist_(0),
            originalMoveSpeed_(0)
    {
    }

    //-Private------------------------------------------------------------------

    void AIEffectCreep::onAwake_()
    {

    }

    void AIEffectCreep::onInit_( GCAttributes* source)
    {
        GCPhysicsController* sourcePhysics;
        if(source)
        {
            source->ComponentByType(Simian::C_TRANSFORM,sourceTransform_);
            source->ComponentByType(GC_PHYSICSCONTROLLER,sourcePhysics);
        }
        // Default
        else
        {
            Simian::Entity* player = Target()->ParentScene()->EntityByType(E_PLAYER);
            if (!player)
            {
                Enable(false);
                return;
            }
            player->ComponentByType(Simian::C_TRANSFORM, sourceTransform_);
            player->ComponentByType(GC_PHYSICSCONTROLLER, sourcePhysics);
        }

        GCPhysicsController* targetPhysics;

        Target()->ParentScene()->EntityByType(E_TILEMAP)->ComponentByType(GC_TILEMAP,map_);
        Target()->ComponentByType(Simian::C_TRANSFORM,targetTransform_);
        Target()->ComponentByType(GC_PHYSICSCONTROLLER,targetPhysics);

        if(!map_ || !targetPhysics || !sourcePhysics || !targetTransform_ || !sourceTransform_)
            Enable(false);
        else
        {
            creepRadiusSqr_ = creepStartDist_ + targetPhysics->Radius() + sourcePhysics->Radius();
            creepRadiusSqr_ *= creepRadiusSqr_;
            stopRadiusSqr_ = creepEndDist_ + targetPhysics->Radius() + sourcePhysics->Radius();
            stopRadiusSqr_ *= stopRadiusSqr_;
        }

        originalMoveSpeed_ = Target()->MovementSpeed();
    }

    void AIEffectCreep::onUnload_()
    {
    }

    //-Public-------------------------------------------------------------------

    bool AIEffectCreep::Update( Simian::f32)
    {
        if(!Enable())
            return false;

        Simian::Vector3 vecToPlayer = sourceTransform_->World().Position() - targetTransform_->World().Position();
        Simian::Vector3 pos = targetTransform_->World().Position()*TileWallCollision::PHYSICS_SCALE;

        if(vecToPlayer.LengthSquared() > stopRadiusSqr_ && vecToPlayer.LengthSquared() < creepRadiusSqr_ && 
           !TileWallCollision::CheckTileCollision(pos,vecToPlayer*TileWallCollision::PHYSICS_SCALE,
            0, &map_->TileMap(),false))
        {
            // DEBUG
            DebugRendererDrawCircle(targetTransform_->World().Position(),std::sqrtf(creepRadiusSqr_),
                Simian::Color(1.0f, 0.0f, 1.0f));
            
            Target()->MovementSpeed(speedRatio_);
            return false;
        }

        if(!OnEnter())
        {
            OnEnter(true);
            Target()->MovementSpeed(originalMoveSpeed_);
        }
        return false;
    }

    void AIEffectCreep::serialize_( Simian::FileObjectSubNode& node )
    {
        node.AddData("SpeedRatio",speedRatio_);
        node.AddData("Dist",std::sqrtf(creepRadiusSqr_));
    }

    void AIEffectCreep::deserialize_( const Simian::FileObjectSubNode& node )
    {
        const Simian::FileDataSubNode* data = node.Data("SpeedRatio");
        speedRatio_ = data? data->AsF32() : speedRatio_;

        data = node.Data("CreepDist");
        creepStartDist_ = data? data->AsF32() : creepStartDist_;

        data = node.Data("StopDist");
        creepEndDist_ = data? data->AsF32() : creepEndDist_;
    }
}
