/*************************************************************************/
/*!
\file		GCSpawnSequence.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCSPAWNSEQUENCE_H_
#define DESCENSION_GCSPAWNSEQUENCE_H_

#include "GCInputListener.h"
#include "ScriptFile.h"
#include "SpawnWave.h"

namespace Descension
{
    class SpawnEvent;
    class GCEnemySpawnSpot;
    class GCAttributes;

    class GCSpawnSequence: public GCInputListener
    {
    private:
        struct Spawner
        {
            std::string Name;
            GCEnemySpawnSpot* SpawnSpot;
        };
    private:
        bool started_;
        bool repeat_;
        std::string scriptLocation_;
        ScriptFile<GCSpawnSequence>* script_;
        std::vector<Spawner> spawners_;
        std::vector<SpawnEvent*> events_;
        std::vector<GCAttributes*> spawnedEntities_;
        Simian::u32 currentEvent_;
        Simian::u32 totalHeat_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCSpawnSequence> factoryPlant_;
    public:
        Simian::u32 TotalHeat() const;
    private:
        // SPAWN FUNCTION CALLBACKS
        void spawnerWave_(Simian::DelegateParameter& p);
        void spawnerTimedWait_(Simian::DelegateParameter& p);
        void spawnerHeatWait_(Simian::DelegateParameter& p);
        void trigger_(Simian::DelegateParameter& p);

        void enemyDeath_(Simian::DelegateParameter& p);

        void update_(Simian::DelegateParameter& p);
    public:
        GCSpawnSequence();

        void OnSharedLoad();
        void OnSharedUnload();

        void OnAwake();
        void OnDeinit();

        void OnTriggered(Simian::DelegateParameter& param);
        void ForceTrigger();
        void KillSpawnedEnemies();
        void ForceStop();

        void Spawn(const std::vector<SpawnWave::SpawnPair>& spawns, Simian::s32 spawnerId);

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
