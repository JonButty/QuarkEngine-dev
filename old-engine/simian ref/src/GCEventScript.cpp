/*************************************************************************/
/*!
\file		GCEventScript.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCEventScript.h"
#include "ComponentTypes.h"
#include "GCEditor.h"

#include "Simian/EngineComponents.h"
#include "Simian/EnginePhases.h"
#include "Simian/LogManager.h"
#include "Simian/Keyboard.h"
#include "Simian/Mouse.h"

#include "sqstdmath.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCEventScript> GCEventScript::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_EVENTSCRIPT);

    GCEventScript::GCEventScript()
        : scriptFile_(new ScriptFile<GCEventScript>()),
          triggerTimer_(0),
          triggerInterval_(1.0f),
          waitTimer_(0),
          triggered_(false),
          repeat_(false),
          disabled_(false),
          skipped_(false)
    {
    }

    GCEventScript::~GCEventScript()
    {
        delete *scriptFile_;
        *scriptFile_ = 0;
    }

    //--------------------------------------------------------------------------

    void GCEventScript::addFunctionToQueue_( const Event& event )
    {
        eventQueue_.push_back(event);
    }

    void GCEventScript::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::Instance().EditorMode())
            return;
#endif
        Simian::EntityUpdateParameter* p;
        param.As(p);

        if (!skipped_ && skipCallback_ != "")
        {
            if (/*Simian::Mouse::IsTriggered(Simian::Mouse::KEY_LBUTTON) ||
                Simian::Mouse::IsTriggered(Simian::Mouse::KEY_RBUTTON) ||
                Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_RETURN) ||
                Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_SPACE) ||*/
                Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_ESCAPE))
            {
                skipped_ = true;

                // end event queue and empty the stack
                eventQueue_.clear();
                stack_.clear();

                // reset any waits..
                waitTimer_ = 0.0f;

                // call the closure
                sq_pushroottable(scriptFile_->VM());
                sq_pushstring(scriptFile_->VM(), skipCallback_.c_str(), -1);
                sq_get(scriptFile_->VM(), -2);
                sq_pushroottable(scriptFile_->VM());
                if (SQ_FAILED(sq_call(scriptFile_->VM(), 1, false, false)))
                {
                    SWarn("Failed to run skip callback : \"" << skipCallback_ << "\".");
                }
                sq_pop(scriptFile_->VM(), 2);
            }
        }

        // run through the event queue as long as im not at the end
        if (eventQueue_.size())
        {
            Parameter eventParam;
            eventParam.Dt = p->Dt;
            eventParam.Event = &eventQueue_[0];
            eventParam.Finished = true;

            // perform function call
            eventQueue_[0].Callback(this, eventParam);

            if (eventParam.Finished)
            {
                // pop the elements of the stack
                stack_.erase(stack_.begin(), stack_.begin() + eventQueue_[0].StackAllocated);

                // pop front of event queue
                eventQueue_.erase(eventQueue_.begin());
            }
        }
        
        if (disabled_ || (triggered_ && !repeat_))
            return;

        triggerTimer_ -= p->Dt;

        if (triggerTimer_ < 0.0f)
        {
            triggerTimer_ = triggerInterval_;
            triggered_ = onTrigger_();
            if (triggered_)
                onExecute_();
        }
    }

    bool GCEventScript::onTrigger_()
    {
        sq_pushroottable(scriptFile_->VM());
        sq_pushstring(scriptFile_->VM(), "OnTrigger", -1);
        if (SQ_FAILED(sq_get(scriptFile_->VM(), -2)))
        {
            sq_pop(scriptFile_->VM(), 1);
            return false;
        }

        sq_pushroottable(scriptFile_->VM());
        if (SQ_FAILED(sq_call(scriptFile_->VM(), 1, true, true)))
        {
            SWarn("Script trigger failed: \"" << scriptLocation_ << "\".");
        }

        SQBool ret;
        sq_getbool(scriptFile_->VM(), -1, &ret);

        sq_pop(scriptFile_->VM(), 2);

        return ret ? true : false;
    }

    void GCEventScript::onExecute_()
    {
        // run the execute function
        sq_pushroottable(scriptFile_->VM());
        sq_pushstring(scriptFile_->VM(), "OnExecute", -1);
        if (SQ_FAILED(sq_get(scriptFile_->VM(), -2)))
        {
            sq_pop(scriptFile_->VM(), 1);
            return;
        }

        // set "this" pointer
        sq_pushroottable(scriptFile_->VM());
        // call "OnExecute" (pops second root table)
        if (SQ_FAILED(sq_call(scriptFile_->VM(), 1, false, true)))
        {
            SWarn("Script execute failed: \"" << scriptLocation_ << "\".");
        }

        sq_pop(scriptFile_->VM(), 2); // removes the function then the root table
    }

    //--------------------------------------------------------------------------

    void GCEventScript::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCEventScript, &GCEventScript::update_>(this));
    }

    void GCEventScript::OnAwake()
    {
#ifdef _DESCENDED
        if (GCEditor::Instance().EditorMode())
            return;
#endif

        ComponentByType(GC_ATTRIBUTES, attributes_);
        ComponentByType(Simian::C_MODEL, model_);

        // register functions
#define REGISTER_FUNCTION(identifier, name) \
    registerQueueFunction_<&GCEventScript::identifier>(name)

        // instant functions
        scriptFile_->RegisterFunction<&GCEventScript::getHealth_>("GetHealth");
        scriptFile_->RegisterFunction<&GCEventScript::getTotalHealth_>("GetTotalHealth");
        scriptFile_->RegisterFunction<&GCEventScript::findEntity_>("FindEntity");
        scriptFile_->RegisterFunction<&GCEventScript::getSelfEntity_>("GetSelfEntity");
        scriptFile_->RegisterFunction<&GCEventScript::cinematicSkipCallback_>("CinematicSkipCallback");
        scriptFile_->RegisterFunction<&GCEventScript::getOSTicks_>("GetOSTicks");

        // queue functions
        REGISTER_FUNCTION(setHealth_, "SetHealth");
        REGISTER_FUNCTION(setForceSeek_, "SetForceSeek");

        REGISTER_FUNCTION(wait_, "Wait");
        REGISTER_FUNCTION(playAnimation_, "PlayAnimation");
        REGISTER_FUNCTION(trigger_, "Trigger");
        REGISTER_FUNCTION(scriptSetGlobal_, "ScriptSetGlobal");
        REGISTER_FUNCTION(setEnabled_, "SetEnabled");
        REGISTER_FUNCTION(setSkip_, "SetSkip");
        REGISTER_FUNCTION(setAIPassive_, "SetAIPassive");
        REGISTER_FUNCTION(setSeekInCine_, "SetSeekInCine");

        REGISTER_FUNCTION(jukeboxPlay_, "JukeboxPlay");
        REGISTER_FUNCTION(jukeboxPlayNext_, "JukeboxPlayNext");
        REGISTER_FUNCTION(jukeboxCrossfade_, "JukeboxCrossfade");

        REGISTER_FUNCTION(soundEmitterPlay_, "SoundEmitterPlay");
        REGISTER_FUNCTION(soundEmitterStopAll_, "SoundEmitterStopAll");

        REGISTER_FUNCTION(particleSystemSetEnabled_, "ParticleSystemSetEnabled");

        REGISTER_FUNCTION(cameraInterpolate_, "CameraInterpolate");

        REGISTER_FUNCTION(cinematicFadeOut_, "CinematicFadeOut");
        REGISTER_FUNCTION(cinematicShowBorder_, "CinematicShowBorder");
        REGISTER_FUNCTION(cinematicHideBorder_, "CinematicHideBorder");
        REGISTER_FUNCTION(cinematicShowSubtitle_, "CinematicShowSubtitle");
        REGISTER_FUNCTION(cinematicShakeScreen_, "CinematicShakeScreen");

        REGISTER_FUNCTION(spriteSetVisible_, "SpriteSetVisible");

        scriptFile_->RegisterFunction<&GCEventScript::entityCreate_>("EntityCreate");
        REGISTER_FUNCTION(entityAdd_, "EntityAdd");
        REGISTER_FUNCTION(entityRemove_, "EntityRemove");
        REGISTER_FUNCTION(entitySetPosition_, "EntitySetPosition");
        REGISTER_FUNCTION(entitySetPositionAtEntity_, "EntitySetPositionAtEntity");
        REGISTER_FUNCTION(entitySetPositionAtEntityYOffset_, "EntitySetPositionAtEntityYOffset");
        REGISTER_FUNCTION(entitySetRotation_, "EntitySetRotation");
        REGISTER_FUNCTION(entityPlayAnimation_, "EntityPlayAnimation");
        REGISTER_FUNCTION(entityModelSetVisible_, "EntityModelSetVisible");
        REGISTER_FUNCTION(entityDealDamage_, "EntityDealDamage");

        REGISTER_FUNCTION(cinematicMotionTranslate_, "CinematicMotionTranslate");
        REGISTER_FUNCTION(cinematicMotionRotate_, "CinematicMotionRotate");

        REGISTER_FUNCTION(playerLockCamera_, "PlayerLockCamera");
        REGISTER_FUNCTION(playerLockMovement_, "PlayerLockMovement");
        REGISTER_FUNCTION(playerRegainMaxHp_, "PlayerRegainMaxHp");
        REGISTER_FUNCTION(playerPlayTeleport_, "PlayerPlayTeleport");
        REGISTER_FUNCTION(playerStopTeleport_, "PlayerStopTeleport");
        REGISTER_FUNCTION(playerSpiritSwirlPlay_, "PlayerSpiritSwirlPlay");
        REGISTER_FUNCTION(playerSpiritSwirlStop_, "PlayerSpiritSwirlStop");
        REGISTER_FUNCTION(playerAddExperience_, "PlayerAddExperience");
		
        REGISTER_FUNCTION(particleTimelinePlay_, "ParticleTimelinePlay");
        REGISTER_FUNCTION(particleTimelineStop_, "ParticleTimelineStop");

        REGISTER_FUNCTION(trapEnable_, "TrapEnable");
        REGISTER_FUNCTION(wallStartDescend_, "WallStartDescend");
        REGISTER_FUNCTION(pathfinderContinuePathfinding_, "PathfinderContinuePathfinding");
        REGISTER_FUNCTION(puzzleLeverTrigger_, "PuzzleLeverTrigger");
        REGISTER_FUNCTION(gestureUnlock_, "GestureUnlock");
        REGISTER_FUNCTION(enablePhysics_, "EnablePhysics");
        REGISTER_FUNCTION(disablePhysics_, "DisablePhysics");
        REGISTER_FUNCTION(canEscapeCinematic_, "CanEscapeCinematic");
        REGISTER_FUNCTION(displayName_, "DisplayName");
        REGISTER_FUNCTION(drawGesture_, "DrawGesture");

        REGISTER_FUNCTION(saveGameData_, "SaveGameData");

        REGISTER_FUNCTION(tilemapSetWall_, "TilemapSetWall");
        REGISTER_FUNCTION(levelChange_, "LevelChange");

#undef REGISTER_FUNCTION

        // register math functions
        sq_pushroottable(scriptFile_->VM());
        sqstd_register_mathlib(scriptFile_->VM());
        sq_pop(scriptFile_->VM(), 1);

        // register global variables
        sq_pushroottable(scriptFile_->VM());

        // push all variables as strings
        for (Simian::u32 i = 0; i < globals_.size(); ++i)
        {
            sq_pushstring(scriptFile_->VM(), globals_[i].first.c_str(), -1);
            sq_pushstring(scriptFile_->VM(), globals_[i].second.c_str(), -1);
            sq_newslot(scriptFile_->VM(), -3, false);
        }

        // pop the root table
        sq_pop(scriptFile_->VM(), 1);

        if (!scriptFile_->Load(this, scriptLocation_))
        {
            SWarn("Script load failed: " << scriptLocation_);
        }
    }

    void DeleteEntityRecursive(Simian::Entity* ent)
    {
        for (Simian::u32 i = 0; i < ent->Children().size(); ++i)
            DeleteEntityRecursive(ent->Children()[i]);
        delete ent;
    }

    void GCEventScript::OnDeinit()
    {
        // delete any entities not added to the scene yet..
        for (Simian::u32 i = 0; i < entities_.size(); ++i)
            DeleteEntityRecursive(entities_[i]);
    }

    void GCEventScript::AddGlobal( const std::string& variable, const std::string& value )
    {
        // predicate for finding global variable
        struct
        {    
            std::string VariableName;
            bool operator()(const std::pair<std::string, std::string>& a)
            {
                return a.first == VariableName;
            }
        } FindByVariableName;
        FindByVariableName.VariableName = variable;

        // add global variable
        std::vector<std::pair<std::string, std::string> >::iterator i =
            std::find_if(globals_.begin(), globals_.end(), FindByVariableName);

        // if not found, create
        if (i == globals_.end())
            globals_.push_back(std::make_pair(variable, value));
        else // otherwise update
            i->second = value;

        // push root table
        sq_pushroottable(scriptFile_->VM());

        // add data
        sq_pushstring(scriptFile_->VM(), variable.c_str(), -1);
        sq_pushstring(scriptFile_->VM(), value.c_str(), -1);
        sq_newslot(scriptFile_->VM(), -3, false);

        // pop root table
        sq_pop(scriptFile_->VM(), 1);
    }

    void GCEventScript::OnTriggered( Simian::DelegateParameter& )
    {
        onExecute_();
    }

    void GCEventScript::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("ScriptFile", scriptLocation_);
        data.AddData("Repeat", repeat_);
        data.AddData("TriggerInterval", triggerInterval_);

        // serialize all globals
        if (globals_.size())
        {
            Simian::FileArraySubNode* globals = data.AddArray("Globals");
            for (Simian::u32 i = 0; i < globals_.size(); ++i)
            {
                Simian::FileObjectSubNode* variable = globals->AddObject("Variable");
                variable->AddData("Name", globals_[i].first);
                variable->AddData("Value", globals_[i].second);
            }
        }
    }

    void GCEventScript::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("ScriptFile", scriptLocation_, scriptLocation_);
        data.Data("Repeat", repeat_, repeat_);
        data.Data("TriggerInterval", triggerInterval_, triggerInterval_);

        // deserialize parameters if there are any
        const Simian::FileArraySubNode* globals = data.Array("Globals");
        if (globals)
        {
            globals_.clear();
            for (Simian::u32 i = 0; i < globals->Size(); ++i)
            {
                const Simian::FileObjectSubNode* globalPair = globals->Object(i);
                std::string name, value;
                globalPair->Data("Name", name, "");
                globalPair->Data("Value", value, "");
                globals_.push_back(std::make_pair(name, value));
            }
        }
    }
}
