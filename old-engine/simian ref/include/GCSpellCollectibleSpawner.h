/*************************************************************************/
/*!
\file		GCSpellCollectibleSpawner.h
\author 	Gavin Yeo, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/

#ifndef SIMIAN_GCSPELLCOLLECTIBLESPAWNER_H_
#define SIMIAN_GCSPELLCOLLECTIBLESPAWNER_H_

#include "Simian/EntityComponent.h"

#include <string>

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCPhysicsController;

    class GCSpellCollectibleSpawner: public Simian::EntityComponent
    {
    private:
        enum SpawnerType
        {
            ST_GENERIC
        };
    private:
        Simian::CTransform* transform_;

        Simian::CTransform* playerCineTransform_;
        GCPhysicsController* playerCinePhysics_;
        Simian::f32 activationRadius_;
        Simian::f32 spawnRadius_;
        Simian::f32 spawnInterval_;
        Simian::f32 spawnHeight_;
        std::string entityName_;
        Simian::s32 entityId_;
        Simian::f32 timer_;
        Simian::u32 spawnAmount_;
        Simian::u32 spawnerType_;

        bool doNotAdd_;

        static bool stopEffect_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCSpellCollectibleSpawner> factoryPlant_;
    public:
        Simian::CTransform* Transform() const;
        void Transform(Simian::CTransform* val);

        Simian::f32 ActivationRadius() const;
        void ActivationRadius(Simian::f32 val);

        Simian::f32 SpawnRadius() const;
        void SpawnRadius(Simian::f32 val);

        Simian::f32 SpawnInterval() const;
        void SpawnInterval(Simian::f32 val);

        Simian::f32 SpawnHeight() const;
        void SpawnHeight(Simian::f32 val);

        std::string EntityName() const;
        void EntityName(std::string val);

        static void StopEffect(bool val);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCSpellCollectibleSpawner();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();

        void Serialize( Simian::FileObjectSubNode& data );
        void Deserialize( const Simian::FileObjectSubNode& data );
    };
}

#endif
