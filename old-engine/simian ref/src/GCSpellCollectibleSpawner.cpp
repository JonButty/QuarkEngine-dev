/*************************************************************************/
/*!
\file		GCSpellCollectibleSpawner.cpp
\author 	Gavin Yeo, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/

#include "GCSpellCollectibleSpawner.h"
#include "GCPlayerLogic.h"
#include "GCEditor.h"
#include "GCAttributes.h"
#include "ComponentTypes.h"
#include "EntityTypes.h"

#include "Simian\Vector3.h"
#include "Simian\CTransform.h"
#include "Simian\EnginePhases.h"
#include "Simian\EngineComponents.h"
#include "Simian\LogManager.h"
#include "Simian\Math.h"
#include "Simian\Scene.h"

namespace Descension
{
    bool GCSpellCollectibleSpawner::stopEffect_ = false;
    Simian::FactoryPlant<Simian::EntityComponent, GCSpellCollectibleSpawner> GCSpellCollectibleSpawner::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_SPELLCOLLECTIBLESPAWNER);

    GCSpellCollectibleSpawner::GCSpellCollectibleSpawner()
        : transform_(0),
          playerCineTransform_(0),
          playerCinePhysics_(0),
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

    Simian::CTransform* GCSpellCollectibleSpawner::Transform() const
    {
        return transform_;
    }

    void GCSpellCollectibleSpawner::Transform( Simian::CTransform* val )
    {
        transform_ = val;
    }

    Simian::f32 GCSpellCollectibleSpawner::ActivationRadius() const
    {
        return activationRadius_;
    }

    void GCSpellCollectibleSpawner::ActivationRadius( Simian::f32 val )
    {
        activationRadius_ = val;
    }

    Simian::f32 GCSpellCollectibleSpawner::SpawnRadius() const
    {
        return spawnRadius_;
    }

    void GCSpellCollectibleSpawner::SpawnRadius( Simian::f32 val )
    {
        spawnRadius_ = val;
    }

    Simian::f32 GCSpellCollectibleSpawner::SpawnInterval() const
    {
        return spawnInterval_;
    }

    void GCSpellCollectibleSpawner::SpawnInterval( Simian::f32 val )
    {
        spawnInterval_ = val;
    }

    Simian::f32 GCSpellCollectibleSpawner::SpawnHeight() const
    {
        return spawnHeight_;
    }

    void GCSpellCollectibleSpawner::SpawnHeight( Simian::f32 val )
    {
        spawnHeight_ = val;
    }

    std::string GCSpellCollectibleSpawner::EntityName() const
    {
        return entityName_;
    }

    void GCSpellCollectibleSpawner::EntityName( std::string val )
    {
        entityName_ = val;
    }

    void GCSpellCollectibleSpawner::StopEffect(bool val)
    {
        stopEffect_ = val;
    }

    //--------------------------------------------------------------------------

    void GCSpellCollectibleSpawner::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::Instance().EditorMode())
            return;
#endif
        // don't do anything if the entity was not found
        if (entityId_ == -1 || stopEffect_)
            return;

        Simian::EntityUpdateParameter* p;
        param.As(p);

        if (!playerCineTransform_)
            return;

        const Simian::Vector3& playerPosition = playerCineTransform_->World().Position();
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

    void GCSpellCollectibleSpawner::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCSpellCollectibleSpawner, &GCSpellCollectibleSpawner::update_>(this));
    }

    void GCSpellCollectibleSpawner::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);

        // find the entity that corresponds with this id.
        if (!Simian::GameFactory::Instance().EntityTable().HasValue(entityName_))
        {
            SWarn("GCSpellCollectibleSpawner cannot find entity: " << entityName_);
            return;
        }
        entityId_ = Simian::GameFactory::Instance().EntityTable().Value(entityName_);
    }

    void GCSpellCollectibleSpawner::OnInit()
    {
        Simian::Entity* playerCine_ = ParentScene()->EntityByTypeRecursive(E_PLAYER_CINE);
        if (playerCine_)
        {
            playerCine_->ComponentByType(Simian::C_TRANSFORM, playerCineTransform_);
            playerCine_->ComponentByType(GC_PHYSICSCONTROLLER, playerCinePhysics_);
        }
    }

    void GCSpellCollectibleSpawner::Serialize( Simian::FileObjectSubNode& data )
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

    void GCSpellCollectibleSpawner::Deserialize( const Simian::FileObjectSubNode& data )
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
