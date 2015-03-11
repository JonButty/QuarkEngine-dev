/*************************************************************************/
/*!
\file		GCEnemyBulletLogic.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"
#include "Simian/CSoundEmitter.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"

#include "GCEnemyBulletLogic.h"
#include "GCPhysicsController.h"
#include "GCPlayerLogic.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCEnemyBulletLogic> GCEnemyBulletLogic::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_ENEMYBULLETLOGIC);

    // Movement and Dodge values
    const Simian::f32 MOVEMENT_SPEED = 12.0f;
    const Simian::f32 BULLET_TIME_MAX = 12.0f;
    const Simian::f32 ROT_SPEED = 0.01f;
    
    GCEnemyBulletLogic::GCEnemyBulletLogic()
        : transform_(0),
          physics_(0),
          timer_(0),
          movementSpeed_(MOVEMENT_SPEED)
    {
    }

    //--------------------------------------------------------------------------

    Simian::CTransform& GCEnemyBulletLogic::Transform()
    {
        return *transform_;
    }

    void GCEnemyBulletLogic::OwnerAttribs(GCAttributes* attribs)
    {
        ownerAttribs_ = attribs;
    }

    Simian::Vector3 GCEnemyBulletLogic::MovementVec() const
    {
        return movementVec_;
    }

    void GCEnemyBulletLogic::MovementVec( Simian::Vector3 val )
    {
        movementVec_ = val;
    }

    //--------------------------------------------------------------------------

    void GCEnemyBulletLogic::update_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        physics_->Velocity(movementVec_);

        timer_ += dt;
        if (timer_ > BULLET_TIME_MAX || physics_->Collided())
        {
            // Hit target
            std::vector<GCPhysicsController*>* hitVec = physics_->GetCollisionList();

            bool hitPlayerButDodging = false;
            for (Simian::u32 i = 0; i < hitVec->size(); ++i)
            {
                // If not player or destructible
                if ((*hitVec)[i] != GCPlayerLogic::Instance()->Physics()
                    && !(*hitVec)[i]->GetBitFlag(GCPhysicsController::PHY_PROP))
                    continue;

                // Get attribs
                GCAttributes* attribs = 0;
                if ((*hitVec)[i] == GCPlayerLogic::Instance()->Physics())
                    attribs = GCPlayerLogic::Instance()->Attributes();
                else
                {
                    Simian::Entity* pe = (*hitVec)[i]->ParentEntity();
                    pe->ComponentByType(GC_ATTRIBUTES, attribs);
                }

                if (attribs)
                {
                    GCPlayerLogic* player = GCPlayerLogic::Instance();
                    if (attribs == player->Attributes())
                    {
                        if (player->DodgeEffective())
                        {
                            hitPlayerButDodging = true;
                            continue;
                        }
                        else
                            GCPlayerLogic::Instance()->Sounds()->Play(GCPlayerLogic::Instance()->S_RANGED_GET_HIT);
                    }

                    CombatCommandSystem::Instance().AddCommand(NULL, attribs,
                        CombatCommandDealDamage::Create(ownerAttribs_->DamageRanged()));
                }
            }
            // Destroy bullet
            if (!hitPlayerButDodging)
            {
                physics_->Enabled(false);
                Simian::Entity* parentEnt = ParentEntity();
                ParentEntity()->ParentScene()->RemoveEntity(parentEnt);
            }
        }
    }

    //--------------------------------------------------------------------------

    void GCEnemyBulletLogic::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCEnemyBulletLogic, &GCEnemyBulletLogic::update_>(this));
    }

    void GCEnemyBulletLogic::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(GC_PHYSICSCONTROLLER, physics_);
    }

    void GCEnemyBulletLogic::OnInit()
    {
        movementVec_ *= movementSpeed_;
    }

    void GCEnemyBulletLogic::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Speed", movementSpeed_);
    }

    void GCEnemyBulletLogic::Deserialize( const Simian::FileObjectSubNode& data )
    {
        // get the data
        const Simian::FileDataSubNode* modelLocation = data.Data("Speed");
        if (modelLocation)
            movementSpeed_ = modelLocation->AsF32();
    }

}
