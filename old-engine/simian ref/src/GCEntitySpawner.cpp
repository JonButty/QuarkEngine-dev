/*************************************************************************/
/*!
\file		GCEntitySpawner.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
            Gavin Yeo, gavin.yeo, 290000111
\par    	email: bryan.yeo\@digipen.edu, gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/

#include "GCEntitySpawner.h"
#include "GCPlayerLogic.h"
#include "GCEditor.h"
#include "GCAttributes.h"
#include "ComponentTypes.h"

#include "Simian\Vector3.h"
#include "Simian\CTransform.h"
#include "Simian\EnginePhases.h"
#include "Simian\EngineComponents.h"
#include "Simian\LogManager.h"
#include "Simian\Math.h"
#include "Simian\Scene.h"

namespace Descension
{
    bool GCEntitySpawner::stopEffect_ = false;
    Simian::FactoryPlant<Simian::EntityComponent, GCEntitySpawner> GCEntitySpawner::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_ENTITYSPAWNER);

    GCEntitySpawner::GCEntitySpawner()
        : transform_(0),
          activationRadius_(0.0f),
          spawnRadius_(0.0f),
          spawnInterval_(0.0f),
          spawnHeight_(0.0f),
          entityId_(-1),
          timer_(0.0f),
          spawnAmount_(1),
          spawnerType_(ST_GENERIC),
          doNotAdd_(false)
    {
    }

    //--------------------------------------------------------------------------

    Simian::CTransform* GCEntitySpawner::Transform() const
    {
        return transform_;
    }

    void GCEntitySpawner::Transform( Simian::CTransform* val )
    {
        transform_ = val;
    }

    Simian::f32 GCEntitySpawner::ActivationRadius() const
    {
        return activationRadius_;
    }

    void GCEntitySpawner::ActivationRadius( Simian::f32 val )
    {
        activationRadius_ = val;
    }

    Simian::f32 GCEntitySpawner::SpawnRadius() const
    {
        return spawnRadius_;
    }

    void GCEntitySpawner::SpawnRadius( Simian::f32 val )
    {
        spawnRadius_ = val;
    }

    Simian::f32 GCEntitySpawner::SpawnInterval() const
    {
        return spawnInterval_;
    }

    void GCEntitySpawner::SpawnInterval( Simian::f32 val )
    {
        spawnInterval_ = val;
    }

    Simian::f32 GCEntitySpawner::SpawnHeight() const
    {
        return spawnHeight_;
    }

    void GCEntitySpawner::SpawnHeight( Simian::f32 val )
    {
        spawnHeight_ = val;
    }

    std::string GCEntitySpawner::EntityName() const
    {
        return entityName_;
    }

    void GCEntitySpawner::EntityName( std::string val )
    {
        entityName_ = val;
    }

    void GCEntitySpawner::StopEffect(bool val)
    {
        stopEffect_ = val;
    }

    //--------------------------------------------------------------------------

    void GCEntitySpawner::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::Instance().EditorMode())
            return;
#endif
        // don't do anything if the entity was not found
        if (entityId_ == -1 || stopEffect_ && spawnerType_ > ST_MANA_ORB)
            return;

        Simian::EntityUpdateParameter* p;
        param.As(p);
        
        GCPlayerLogic* player = GCPlayerLogic::Instance();
        if (!doNotAdd_)
        {
            if (spawnerType_ != ST_GENERIC)
            {
                if ((spawnerType_ == ST_HEALTH_ORB && player->Attributes()->Health() >= player->Attributes()->MaxHealth())
                    || (spawnerType_ == ST_MANA_ORB && ((player->Attributes()->MaxMana()-player->Attributes()->Mana()) < Simian::Math::EPSILON )))
                    return;
            }
        }

        const Simian::Vector3& playerPosition = player->Transform().World().Position();
        if ((playerPosition - transform_->World().Position()).LengthSquared() < activationRadius_ * activationRadius_)
        {
            // spawner is active
            timer_ += p->Dt;
            if (timer_ > spawnInterval_)
            {
                timer_ = 0.0f;
                
                for (Simian::u32 i = 0; i < spawnAmount_; ++i)
                {
                    // spawn an entity
                    Simian::Entity* entity = ParentScene()->CreateEntity(entityId_);
                    if (!entity)
                        return; // failed to spawn

                    // get the transform
                    Simian::CTransform* transform;
                    entity->ComponentByType(Simian::C_TRANSFORM, transform);

                    if (transform)
                    {
                        // randomize an angle
                        Simian::f32 angle = Simian::Math::Random(0.0f, Simian::Math::TAU);
                        Simian::Vector3 direction(std::cos(angle), 0.0f, std::sin(angle));

                        // set the position
                        transform->Position(transform_->Position() + direction * 
                            spawnRadius_ * Simian::Math::Random(0.0f, 1.0f) + 
                            Simian::Vector3(0, Simian::Math::Random(0.0f, spawnHeight_), 0));
                    }

                    // add spawned entity to scene
                    ParentScene()->AddEntity(entity);
                }
            }
        }
    }

    //--------------------------------------------------------------------------

    void GCEntitySpawner::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCEntitySpawner, &GCEntitySpawner::update_>(this));
    }

    void GCEntitySpawner::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);

        // find the entity that corresponds with this id.
        if (!Simian::GameFactory::Instance().EntityTable().HasValue(entityName_))
        {
            SWarn("GCEntitySpawner cannot find entity: " << entityName_);
            return;
        }
        entityId_ = Simian::GameFactory::Instance().EntityTable().Value(entityName_);
    }

    void GCEntitySpawner::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("ActivationRadius", activationRadius_);
        data.AddData("SpawnRadius", spawnRadius_);
        data.AddData("SpawnInterval", spawnInterval_);
        data.AddData("SpawnHeight", spawnHeight_);
        data.AddData("EntityName", entityName_);
        data.AddData("SpawnAmount", spawnAmount_);
        data.AddData("SpawnerType", spawnerType_);

        data.AddData("DoNotAdd", doNotAdd_);
    }

    void GCEntitySpawner::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("ActivationRadius", activationRadius_, activationRadius_);
        data.Data("SpawnRadius", spawnRadius_, spawnRadius_);
        data.Data("SpawnInterval", spawnInterval_, spawnInterval_);
        data.Data("SpawnHeight", spawnHeight_, spawnHeight_);
        data.Data("EntityName", entityName_, entityName_);
        data.Data("SpawnAmount", spawnAmount_, spawnAmount_);

        const Simian::FileDataSubNode* ml = data.Data("");
        ml = data.Data("SpawnerType");
        ml ? spawnerType_ = ml->AsU32() : 0;

        data.Data("DoNotAdd", doNotAdd_, doNotAdd_);
    }
}
