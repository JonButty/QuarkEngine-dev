/******************************************************************************/
/*!
\file		GCAIDestructible.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/Debug.h"
#include "Simian/CModel.h"
#include "Simian/CTransform.h"
#include "Simian/AnimationController.h"
#include "Simian/CSoundEmitter.h"
#include "Simian/Scene.h"
#include "Simian/LogManager.h"
#include "GCDescensionCamera.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"
#include "EntityCollision.h"
#include "PathFinder.h"
#include "GCPlayerLogic.h"

#include "GCPhysicsController.h"
#include "GCAIDestructible.h"
#include "GCAttributes.h"
#include "GCEditor.h"

#include <iostream>
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCAIDestructible> factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_AIDESTRUCTIBLE);

    GCAIDestructible::GCAIDestructible()
        :   GCAIBase(ES_IDLE, ES_TOTAL),
            animFrameRate_(48),
            transform_(0),
            playerTransform_(0),
            sounds_(0),
            totalDestroySounds_(5),
            reactDmg_(-1),
            reactDmg2_(-1),
            explosionDamage_(20),
            explosionRadius_(1.5f)
    { 
    }

    GCAIDestructible::~GCAIDestructible()
    {

    }

    //-Public-------------------------------------------------------------------

    void GCAIDestructible::Deserialize( const Simian::FileObjectSubNode& data )
    {
        GCAIBase::Deserialize(data);

        const Simian::FileDataSubNode* temp = data.Data("AnimationFrameRate");
        animFrameRate_ = temp? static_cast<Simian::u32>(temp->AsF32()) : animFrameRate_;

        data.Data("ReactDamage", reactDmg_, reactDmg_);
        data.Data("ReactDamage2", reactDmg2_, reactDmg2_);
        data.Data("ExplosionDamage", explosionDamage_, explosionDamage_);
        data.Data("ExplosionRadius", explosionRadius_, explosionRadius_);
    }

    void GCAIDestructible::Serialize( Simian::FileObjectSubNode& data)
    {
        GCAIBase::Serialize(data);
        data.AddData("AnimationFrameRate",animFrameRate_);

        data.AddData("ReactDamage", reactDmg_);
        data.AddData("ReactDamage2", reactDmg2_);
        data.AddData("ExplosionDamage", explosionDamage_);
        data.AddData("ExplosionRadius", explosionRadius_);
    }

    void GCAIDestructible::OnAwake()
    {
        GCAIBase::OnAwake();

        aiType_ = AI_PASSIVE;

        fsm_[ES_IDLE].OnEnter = Simian::Delegate::CreateDelegate<GCAIDestructible, &GCAIDestructible::idleOnEnter_>(this);
        fsm_[ES_IDLE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIDestructible, &GCAIDestructible::idleOnUpdate_>(this);
        fsm_[ES_DEATH].OnEnter = Simian::Delegate::CreateDelegate<GCAIDestructible, &GCAIDestructible::deathOnEnter_>(this);
        fsm_[ES_DEATH].OnUpdate = Simian::Delegate::CreateDelegate<GCAIDestructible, &GCAIDestructible::deathOnUpdate_>(this);
    }

    void GCAIDestructible::OnInit()
    {
        GCAIBase::OnInit();
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);
        ComponentByType(Simian::C_TRANSFORM, transform_);
        GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,playerTransform_);

#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
    }

    void GCAIDestructible::OnDeinit()
    {
        GCAIBase::OnDeinit();
    }

    void GCAIDestructible::OnSharedLoad()
    {
        GCAIBase::OnSharedLoad();
    }

    //-Private------------------------------------------------------------------

    void GCAIDestructible::idleOnEnter_( Simian::DelegateParameter&)
    {
        model_->Controller().PlayAnimation("Idle", false);
    }

    void GCAIDestructible::idleOnUpdate_( Simian::DelegateParameter&)
    {
        PathFinder::Instance().SetNewPosition(transform_->World().Position(),physics_->Radius(),oldMin_,oldMax_);
        if(attributes_->Health() <= 0)
            fsm_.ChangeState(ES_DEATH);
        else if ((reactDmg_ > 0 &&
                 attributes_->Health() == attributes_->MaxHealth()-reactDmg_) ||
                 (reactDmg2_ > 0 &&
                 attributes_->Health() == attributes_->MaxHealth()-reactDmg2_))
        {            
            // Create explosion
            Simian::Entity* explosion = ParentEntity()->ParentScene()->CreateEntity(E_FIREBARRELEXPLOSION);
            ParentEntity()->ParentScene()->AddEntity(explosion);
            // Set explosion transform
            Simian::CTransform* expTransform_;
            explosion->ComponentByType(Simian::C_TRANSFORM, expTransform_);
            expTransform_->Position(Simian::Vector3(transform_->World().Position().X(),
                transform_->World().Position().Y(), transform_->World().Position().Z()));
            
            // Shake screen
            GCDescensionCamera::Instance()->Shake(0.35f, 0.35f);

            // Destroy enemies around it
            Simian::u32 enemyNum = GCAIBase::AllEnemiesVec().size();
            // Loop through all enemies
            for (Simian::u32 i=0; i < enemyNum; ++i)
            {
                // Get vector to enemy
                GCAIBase* enemy = GCAIBase::AllEnemiesVec()[i];
                Simian::Vector3 meToEnemy = enemy->Transform()->WorldPosition() - transform_->Position();
                Simian::f32 meToELength = meToEnemy.Length() - physics_->Radius() - enemy->Radius();

                // Check if distance to enemy is within attack range
                if (meToELength <= explosionRadius_)
                {
                    GCAttributes* attribs;
                    GCAIBase::AllEnemiesVec()[i]->ComponentByType(GC_ATTRIBUTES, attribs);
                    if (attribs && attribs->Health() > 0)
                    {
                        // deal damage if the enemy has attributes
                        CombatCommandSystem::Instance().AddCommand(attributes_, attribs,
                            CombatCommandDealDamage::Create(explosionDamage_));
                    }
                }
            }

            // Deal damage to player if he is near it
            Simian::Vector3 vecToTarget = transform_->World().Position() - playerTransform_->World().Position();
            Simian::f32 distance = vecToTarget.LengthSquared();
            if(distance <= explosionRadius_)
            {
                CombatCommandSystem::Instance().AddCommand(attributes_, GCPlayerLogic::Instance()->Attributes(),
                    CombatCommandDealDamage::Create(explosionDamage_ * 10));
            }

            fsm_.ChangeState(ES_DEATH);
        }

        if (reactDmg_ > 0 &&
            attributes_->Health() < attributes_->MaxHealth())
            attributes_->Health(attributes_->MaxHealth());
    }

    void GCAIDestructible::deathOnEnter_( Simian::DelegateParameter&  )
    {
        model_->Controller().PlayAnimation("Die", false, animFrameRate_);
        physics_->Enabled(false);
        PathFinder::Instance().UnsetPosition(oldMin_,oldMax_);
        attributes_->Health(0);
        // Play random destroy sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(0, totalDestroySounds_));
    }

    void GCAIDestructible::deathOnUpdate_( Simian::DelegateParameter&  )
    {
    }

    void GCAIDestructible::overRideStateUpdate_( Simian::f32 dt )
    {
        dt;
    }
}
