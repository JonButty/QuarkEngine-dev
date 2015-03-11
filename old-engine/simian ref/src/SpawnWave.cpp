/*************************************************************************/
/*!
\file		SpawnWave.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "SpawnWave.h"
#include "SpawnEventTypes.h"
#include "GCSpawnSequence.h"

#include "Simian/LogManager.h"
#include "Simian/GameFactory.h"

namespace Descension
{
    SpawnFactory::Plant<SpawnWave>::Type SpawnWave::factoryPlant_(
        SpawnFactory::InstancePtr(), SE_WAVE);

    SpawnWave::SpawnWave()
        : spawnerId_(SI_NOID)
    {
    }

    //--------------------------------------------------------------------------

    Simian::s32 SpawnWave::SpawnerId() const
    {
        return spawnerId_;
    }

    void SpawnWave::SpawnerId( Simian::s32 val )
    {
        spawnerId_ = val;
    }

    //--------------------------------------------------------------------------

    void SpawnWave::AddSpawn( const std::string& entityName, Simian::u32 spawnAmount )
    {
        // resolve entity name
        if (!Simian::GameFactory::Instance().EntityTable().HasValue(entityName))
        {
            SWarn(std::string("Invalid spawn entity name: ") + entityName);
            return;
        }

        Simian::s32 entityId = Simian::GameFactory::Instance().EntityTable().Value(entityName);
        AddSpawn(entityId, spawnAmount);
    }

    void SpawnWave::AddSpawn( Simian::s32 entityId, Simian::u32 spawnAmount )
    {
        SpawnPair pair;
        pair.EntityId = entityId;
        pair.SpawnAmount = spawnAmount;
        spawnPairs_.push_back(pair);
    }

    bool SpawnWave::Execute(Simian::f32)
    {
        sequence_->Spawn(spawnPairs_, spawnerId_);
        return true;
    }
}
