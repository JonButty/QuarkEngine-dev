/*************************************************************************/
/*!
\file		GCEnemyDrop.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCEnemyDrop.h"
#include "ComponentTypes.h"
#include "GCAttributes.h"
#include "EntityTypes.h"
#include "GCCollectible.h"

#include "Simian/EngineComponents.h"
#include "Simian/Math.h"
#include "Simian/CTransform.h"
#include "Simian/CModel.h"
#include "Simian/Model.h"
#include "Simian/Scene.h"

#include <cmath>

namespace Descension
{
    static const Simian::f32 PROJECTILE_MAX_HEIGHT = 0.3f;
    static const Simian::f32 Y_VEL_SQUARE = 2.0f * GRAVITATIONAL_ACCELERATION * PROJECTILE_MAX_HEIGHT;
    static const Simian::f32 INITIAL_Y_VELOCITY = std::sqrt(Y_VEL_SQUARE);
    static const Simian::f32 RADIUS_PER_DROP = 0.05f;

    Simian::FactoryPlant<Simian::EntityComponent, GCEnemyDrop> GCEnemyDrop::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_ENEMYDROP);

    // t computation
    /*
        t = (vy0 + sqrt(2gh + 2gy0))/g;
        v = (x - x0)/t; (2d "floor" velocity) x -> final pos, x0 -> enemy pos
    */

    GCEnemyDrop::GCEnemyDrop()
        : healthDrop_(0),
          randomHealthDrop_(0),
          manaDrop_(0),
          randomManaDrop_(0),
          expDrop_(0),
          randomExpDrop_(0),
          totalDrops_(0),
          attributes_(0),
          transform_(0)
    {
    }
    
    //--------------------------------------------------------------------------

    void GCEnemyDrop::animateDrop_( Simian::Entity* drop )
    {
        Simian::CTransform* transform;
        GCCollectible* collectible;

        drop->ComponentByType(Simian::C_TRANSFORM, transform);
        drop->ComponentByType(GC_COLLECTIBLE, collectible);

        Simian::f32 angle = Simian::Math::Random(0.0f, Simian::Math::TAU);
        Simian::Vector3 dir(std::cos(angle), 0, std::sin(angle));
        Simian::f32 length = Simian::Math::Random(0.0f, RADIUS_PER_DROP * totalDrops_);
        dir *= length; // scale direction by length
        
        Simian::f32 hHeight = model_ ? (model_->Model().Max().Y() - model_->Model().Min().Y()) * 0.5f : 0.0f;
        transform->Position(transform_->World().Position() + 
            Simian::Vector3(0, hHeight, 0));
        
        // find dir factor
        dir /=  (INITIAL_Y_VELOCITY + std::sqrt(Y_VEL_SQUARE + 2.0f * GRAVITATIONAL_ACCELERATION * hHeight))/GRAVITATIONAL_ACCELERATION;
        dir += Simian::Vector3(0, INITIAL_Y_VELOCITY, 0);

        // dir is now the velocity
        collectible->Velocity(dir);
    }

    void GCEnemyDrop::onDeath_( Simian::DelegateParameter& )
    {
        // spawn health and mana!
        Simian::u32 healthDrops = healthDrop_ + (randomHealthDrop_ ? Simian::Math::Random(0, randomHealthDrop_) : 0);
        Simian::u32 manaDrops = manaDrop_ + (randomManaDrop_ ? Simian::Math::Random(0, randomManaDrop_) : 0);

        totalDrops_ = healthDrops + manaDrops;

        while (healthDrops--)
        {
            // spawn a health globule
            Simian::Entity* e = ParentScene()->CreateEntity(E_HEALTHSHARD);
            animateDrop_(e);
            ParentScene()->AddEntity(e);
        }

        while (manaDrops--)
        {
            // spawn a mana globule
            Simian::Entity* e = ParentScene()->CreateEntity(E_MANASHARD);
            animateDrop_(e);
            ParentScene()->AddEntity(e);
        }
    }

    //--------------------------------------------------------------------------

    void GCEnemyDrop::OnAwake()
    {
        ComponentByType(GC_ATTRIBUTES, attributes_);
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_MODEL, model_);

        // register death callback
        attributes_->DeathDelegates() += Simian::Delegate::CreateDelegate<GCEnemyDrop, &GCEnemyDrop::onDeath_>(this);
    }

    void GCEnemyDrop::OnDeinit()
    {
    }

    void GCEnemyDrop::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("HealthDrop", healthDrop_);
        data.AddData("RandomHealthDrop", randomHealthDrop_);
        data.AddData("ManaDrop", manaDrop_);
        data.AddData("RandomManaDrop", randomManaDrop_);
        data.AddData("ExpDrop", expDrop_);
        data.AddData("RandomExpDrop", randomExpDrop_);
    }

    void GCEnemyDrop::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("HealthDrop", healthDrop_, healthDrop_);
        data.Data("RandomHealthDrop", randomHealthDrop_, randomHealthDrop_);
        data.Data("ManaDrop", manaDrop_, manaDrop_);
        data.Data("RandomManaDrop", randomManaDrop_, randomManaDrop_);
        data.Data("ExpDrop", expDrop_, expDrop_);
        data.Data("RandomExpDrop", randomExpDrop_, randomExpDrop_);
    }
}
