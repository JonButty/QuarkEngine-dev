/*************************************************************************/
/*!
\file		GCEntitySpawner.h
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

#ifndef SIMIAN_GCENTITYSPAWNER_H_
#define SIMIAN_GCENTITYSPAWNER_H_

#include "Simian/EntityComponent.h"

#include <string>

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCEntitySpawner: public Simian::EntityComponent
    {
    private:
        enum SpawnerType
        {
            ST_GENERIC,
            ST_HEALTH_ORB,
            ST_MANA_ORB,
            ST_HEALTH_ORB_NA,
            ST_MANA_ORB_NA,
            ST_EXP_ORB_NA
        };
    private:
        Simian::CTransform* transform_;

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

        static Simian::FactoryPlant<Simian::EntityComponent, GCEntitySpawner> factoryPlant_;
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
        GCEntitySpawner();

        void OnSharedLoad();
        void OnAwake();

        void Serialize( Simian::FileObjectSubNode& data );
        void Deserialize( const Simian::FileObjectSubNode& data );
    };
}

#endif
