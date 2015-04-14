/*************************************************************************/
/*!
\file		GCPlayerBulletLogic.cpp
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

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"

#include "GCPlayerBulletLogic.h"
#include "GCPhysicsController.h"
#include "GCAIBase.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCPlayerBulletLogic> GCPlayerBulletLogic::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_PLAYERBULLETLOGIC);

    // Movement and Dodge values
    const Simian::f32 MOVEMENT_SPEED = 12.0f;
    const Simian::f32 BULLET_TIME_MAX = 4.0f;
    
    GCPlayerBulletLogic::GCPlayerBulletLogic()
        : transform_(0),
          physics_(0),
          timer_(0),
          ownerAttribs_(0),
          movementSpeed_(MOVEMENT_SPEED)
    {
    }

    //--------------------------------------------------------------------------

    Simian::CTransform& GCPlayerBulletLogic::Transform()
    {
        return *transform_;
    }

    void GCPlayerBulletLogic::OwnerAttribs(GCAttributes* attribs)
    {
        ownerAttribs_ = attribs;
    }

    //--------------------------------------------------------------------------

    void GCPlayerBulletLogic::update_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        physics_->Velocity(movementVec_);

        timer_ += dt;
        if (timer_ > BULLET_TIME_MAX || physics_->Collided())
        {
            // Hit target
            std::vector<GCPhysicsController*>* hitVec = physics_->GetCollisionList();
            for (Simian::u32 i = 0; i < hitVec->size(); ++i)
            {
                //GCAIBase* pel;
                GCAttributes* attribs;
                Simian::Entity* pe = (*hitVec)[i]->ParentEntity();
                // If enemy
                pe->ComponentByType(GC_ATTRIBUTES, attribs);
                if (attribs)
                {
                    CombatCommandSystem::Instance().AddCommand(NULL, attribs,
                        CombatCommandDealDamage::Create(ownerAttribs_->DamageRanged()));
                }
            }
            // Destroy bullet
            physics_->Enabled(false);
            Simian::Entity* parentEnt = ParentEntity();
            ParentEntity()->ParentScene()->RemoveEntity(parentEnt);
        }
    }

    //--------------------------------------------------------------------------

    void GCPlayerBulletLogic::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCPlayerBulletLogic, &GCPlayerBulletLogic::update_>(this));
    }

    void GCPlayerBulletLogic::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(GC_PHYSICSCONTROLLER, physics_);
    }

    void GCPlayerBulletLogic::OnInit()
    {
        movementVec_ = Simian::Vector3(std::cos(transform_->Angle().Radians()), 0, std::sin(transform_->Angle().Radians())) * movementSpeed_;
        //physics_->CollideWithVoid(false); //only use for bullets
    }

    void GCPlayerBulletLogic::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Speed", movementSpeed_);
    }

    void GCPlayerBulletLogic::Deserialize( const Simian::FileObjectSubNode& data )
    {
        // get the data
        const Simian::FileDataSubNode* modelLocation = data.Data("Speed");
        if (modelLocation)
            movementSpeed_ = modelLocation->AsF32();
    }
}
