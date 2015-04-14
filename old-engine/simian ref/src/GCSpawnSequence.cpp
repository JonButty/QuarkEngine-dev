/*************************************************************************/
/*!
\file		GCSpawnSequence.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCSpawnSequence.h"
#include "ComponentTypes.h"
#include "SpawnFactory.h"
#include "SpawnWave.h"
#include "SpawnEventTypes.h"
#include "GCEditor.h"
#include "GCEnemySpawnSpot.h"
#include "SpawnTimedWait.h"
#include "GCAttributes.h"
#include "SpawnHeatWait.h"
#include "TriggerEvent.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"

#include "Simian/LogManager.h"
#include "Simian/Scene.h"
#include "Simian/EnginePhases.h"
#include "Simian/Math.h"

#include "squirrel.h"

#include <algorithm>

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCSpawnSequence> GCSpawnSequence::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_SPAWNSEQUENCE);

    GCSpawnSequence::GCSpawnSequence()
        : started_(false),
          repeat_(false),
          script_(0),
          currentEvent_(0),
          totalHeat_(0)
    {
    }

    //--------------------------------------------------------------------------

    Simian::u32 GCSpawnSequence::TotalHeat() const
    {
        return totalHeat_;
    }

    //--------------------------------------------------------------------------


    void GCSpawnSequence::spawnerWave_( Simian::DelegateParameter& p )
    {
        ScriptFile<GCSpawnSequence>::CallParameter* param;
        p.As(param);

        SpawnWave* wave;
        SpawnFactory::Instance().CreateInstance(SE_WAVE, wave);
        wave->Sequence(this);

        // first parameter is the spawner id
        Simian::s32 spawnerId;
        sq_getinteger(param->VM, 2, &spawnerId);
        wave->SpawnerId(spawnerId);

        // get the objects in the function parameters
        Simian::s32 params = sq_gettop(param->VM);

        // get the wave at this position
        for (int i = 3; i <= params; ++i)
        {
            // take the params out of it (first is the name)
            sq_pushinteger(param->VM, 0);
            sq_get(param->VM, i);

            const SQChar* name;
            sq_getstring(param->VM, -1, &name);

            // take second parameter 
            sq_pushinteger(param->VM, 1);
            sq_get(param->VM, i);

            Simian::s32 spawnAmount;
            sq_getinteger(param->VM, -1, &spawnAmount);

            wave->AddSpawn(std::string(name), spawnAmount);
        }

        events_.push_back(wave);
    }

    void GCSpawnSequence::spawnerTimedWait_( Simian::DelegateParameter& p )
    {
        ScriptFile<GCSpawnSequence>::CallParameter* param;
        p.As(param);

        SpawnTimedWait* wait;
        SpawnFactory::Instance().CreateInstance(SE_TIMEDWAIT, wait);
        wait->Sequence(this);

        // get the amount of time to wait
        SQFloat time;
        sq_getfloat(param->VM, 2, &time);

        wait->Reset(time);

        events_.push_back(wait);
    }

    void GCSpawnSequence::spawnerHeatWait_( Simian::DelegateParameter& p )
    {
        ScriptFile<GCSpawnSequence>::CallParameter* param;
        p.As(param);

        SpawnHeatWait* wait;
        SpawnFactory::Instance().CreateInstance(SE_HEATWAIT, wait);
        wait->Sequence(this);

        SQInteger heat;
        sq_getinteger(param->VM, 2, &heat);

        wait->Heat(heat);
        events_.push_back(wait);
    }

    void GCSpawnSequence::trigger_( Simian::DelegateParameter& p )
    {
        ScriptFile<GCSpawnSequence>::CallParameter* param;
        p.As(param);

        TriggerEvent* trigger;
        SpawnFactory::Instance().CreateInstance(SE_TRIGGER, trigger);

        const SQChar* listener;
        sq_getstring(param->VM, 2, &listener);

        const SQChar* target = "";
        if (sq_gettop(param->VM) >= 3)
            sq_getstring(param->VM, 3, &target);

        trigger->ResolveTal(ParentEntity(), listener, target);
        events_.push_back(trigger);
    }

    void GCSpawnSequence::enemyDeath_( Simian::DelegateParameter& p )
    {
        GCAttributes::DeathParameter* param;
        p.As(param);
        SAssert(totalHeat_ >= param->Target->Heat(), "Heat going into the negative!");
        totalHeat_ -= !totalHeat_ ? 0 : param->Target->Heat();

        // remove from spawned enemies
        std::vector<GCAttributes*>::iterator i = std::find(spawnedEntities_.begin(),
            spawnedEntities_.end(), param->Target);
        if (i != spawnedEntities_.end())
            spawnedEntities_.erase(i);
    }

    void GCSpawnSequence::update_( Simian::DelegateParameter& p )
    {
        Simian::EntityUpdateParameter* param;
        p.As(param);

        if (currentEvent_ != events_.size())
            currentEvent_ += events_[currentEvent_]->Execute(param->Dt) ? 1 : 0;
    }

    //--------------------------------------------------------------------------

    void GCSpawnSequence::OnSharedLoad()
    {
        // script is a shared resource..
        script_ = new ScriptFile<GCSpawnSequence>();

        script_->RegisterFunction<&GCSpawnSequence::spawnerWave_>("SpawnerWave");
        script_->RegisterFunction<&GCSpawnSequence::spawnerTimedWait_>("SpawnerTimedWait");
        script_->RegisterFunction<&GCSpawnSequence::spawnerHeatWait_>("SpawnerHeatWait");
        script_->RegisterFunction<&GCSpawnSequence::trigger_>("Trigger");

        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCSpawnSequence, &GCSpawnSequence::update_>(this));
    }

    void GCSpawnSequence::OnSharedUnload()
    {
        // delete the script
        delete script_;
    }

    void GCSpawnSequence::OnAwake()
    {
#ifdef _DESCENDED
        if (GCEditor::Instance().EditorMode())
            return;
#endif
        currentEvent_ = 0;

        // resolve spawner names
        for (Simian::u32 i = 0; i < spawners_.size(); ++i)
        {
            Simian::Entity* entity = ParentScene()->EntityByNameRecursive(spawners_[i].Name);
            if (!entity)
            {
                // invalid spawner..
                SWarn(std::string("Spawner does not exist: ") + spawners_[i].Name);
                continue;
            }
            entity->ComponentByType(GC_ENEMYSPAWNSPOT, spawners_[i].SpawnSpot);
        }

        // initially started?
        if (started_)
            script_->Do(this, scriptLocation_);
    }

    void GCSpawnSequence::OnDeinit()
    {
        // destroy all events
        for (Simian::u32 i = 0; i < events_.size(); ++i)
            SpawnFactory::Instance().DestroyInstance(events_[i]);
        events_.clear();
    }

    void GCSpawnSequence::OnTriggered( Simian::DelegateParameter& )
    {
#ifdef _DESCENDED
        if (GCEditor::Instance().EditorMode())
            return;
#endif

        if (started_ && !repeat_)
            return;

        // begin waves!
        started_ = true;
        
        // run script
        script_->Do(this, scriptLocation_);
    }

    void GCSpawnSequence::ForceTrigger()
    {
#ifdef _DESCENDED
        if (GCEditor::Instance().EditorMode())
            return;
#endif

        // begin waves!
        started_ = true;

        // run script
        script_->Do(this, scriptLocation_);
    }

    void GCSpawnSequence::KillSpawnedEnemies()
    {
        for (Simian::u32 i = 0; i < spawnedEntities_.size(); ++i)
        {
            CombatCommandSystem::Instance().AddCommand(NULL, spawnedEntities_[i], 
                CombatCommandDealDamage::Create(spawnedEntities_[i]->Health()));
        }
        spawnedEntities_.clear();
    }

    void GCSpawnSequence::ForceStop()
    {
        // go immediately to the event size
        currentEvent_ = events_.size();
    }

    void GCSpawnSequence::Spawn( const std::vector<SpawnWave::SpawnPair>& spawns, Simian::s32 spawnerId )
    {
        spawnerId = spawnerId >= 0 && static_cast<Simian::u32>(spawnerId) >= spawners_.size() ? -1 : spawnerId;
        spawnerId = spawnerId < 0 ? Simian::Math::Random(0, spawners_.size()) : spawnerId;

        for (Simian::u32 i = 0; i < spawns.size(); ++i)
        {
            for (Simian::u32 j = 0; j < spawns[i].SpawnAmount; ++j)
            {
                GCAttributes* attribs = spawners_[spawnerId].SpawnSpot->Spawn(spawns[i].EntityId);
                totalHeat_ += attribs ? attribs->Heat() : 0;
                attribs ? attribs->DeathDelegates() += 
                    Simian::Delegate::CreateDelegate<GCSpawnSequence, &GCSpawnSequence::enemyDeath_>(this) : 0;
                spawnedEntities_.push_back(attribs);
            }
        }
    }

    void GCSpawnSequence::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Started", started_);
        data.AddData("Repeat", repeat_);
        data.AddData("SpawnScript", scriptLocation_);

        Simian::FileArraySubNode* spawnersNode = data.AddArray("Spawners");
        for (Simian::u32 i = 0; i < spawners_.size(); ++i)
            spawnersNode->AddData("Spawner", spawners_[i].Name);
    }

    void GCSpawnSequence::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("Started", started_, started_);
        data.Data("Repeat", repeat_, repeat_);
        data.Data("SpawnScript", scriptLocation_, scriptLocation_);

        const Simian::FileArraySubNode* spawnersNode = data.Array("Spawners");
        if (spawnersNode)
        {
            spawners_.clear();
            for (Simian::u32 i = 0; i < spawnersNode->Size(); ++i)
            {
                std::string spawnerName = spawnersNode->Data(i)->AsString();
                Spawner spawner = { spawnerName, 0 };
                spawners_.push_back(spawner);
            }
        }
    }
}
