/*************************************************************************/
/*!
\file		GCEventScriptFunctions.cpp
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
#include "GCAttributes.h"
#include "GCTileMap.h"
#include "TileMap.h"
#include "Pathfinder.h"
#include "GCInputListener.h"
#include "StateLoading.h"
#include "StateGame.h"
#include "ComponentTypes.h"
#include "GCMousePlaneIntersection.h"
#include "EntityTypes.h"
#include "GCPlayerLogic.h"
#include "GCDescensionCamera.h"
#include "GCCinematicUI.h"
#include "GCTrapLaserBeam.h"
#include "GCTrapFireSpot.h"
#include "GCGesturePuzzle.h"
#include "GCGestureInterpreter.h"
#include "GCMovablePlatform.h"
#include "GCUI.h"
#include "GCPhysicsController.h"
#include "GCCinematicEscape.h"
#include "GCCinematicMotion.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"
#include "CombatCommandHeal.h"
#include "GCAIGeneric.h"
#include "GCDescendingWall.h"
#include "GCGameData.h"
#include "PathFinder.h"
#include "GestureTutor.h"
#include "SaveManager.h"

#include "Simian/Scene.h"
#include "Simian/EngineComponents.h"
#include "Simian/EnginePhases.h"
#include "Simian/CModel.h"
#include "Simian/AnimationController.h"
#include "Simian/LogManager.h"
#include "Simian/Game.h"
#include "Simian/CJukebox.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian/CSoundEmitter.h"
#include "Simian/CSprite.h"
#include "Simian/CParticleTimeline.h"

namespace Descension
{
    static void interpretInterpolator_(Simian::f32(*&interpolator)(Simian::f32), const std::string& interpolationType)
    {
        interpolator = 
            interpolationType == "Linear" ? Simian::Interpolation::Linear : 
            interpolationType == "SquareIn" ? Simian::Interpolation::EaseSquareIn : 
            interpolationType == "SquareOut" ? Simian::Interpolation::EaseSquareOut : 
            interpolationType == "SquareInOut" ? Simian::Interpolation::EaseSquareInOut : 
            interpolationType == "CubicIn" ? Simian::Interpolation::EaseCubicIn : 
            interpolationType == "CubicOut" ? Simian::Interpolation::EaseCubicOut : 
            interpolationType == "CubicInOut" ? Simian::Interpolation::EaseCubicInOut : 
            Simian::Interpolation::Linear;
    }

    void GCEventScript::findEntity_( Simian::DelegateParameter& param )
    {
        ScriptFile<GCEventScript>::CallParameter* p;
        param.As(p);
        const SQChar* s;
        sq_getstring(p->VM, 2, &s);
        Simian::Entity* entity = ParentScene()->EntityByNameRecursive(s);
        if (!entity)
        {
            SWarn("Could not find entity: \"" << s << "\"");
        }
        sq_pushuserpointer(p->VM, (SQUserPointer)entity);
        p->ReturnValue = true;
    }

    void GCEventScript::getSelfEntity_( Simian::DelegateParameter& param )
    {
        ScriptFile<GCEventScript>::CallParameter* p;
        param.As(p);
        sq_pushuserpointer(p->VM, attributes_->ParentEntity());
        p->ReturnValue = true;
    }

    void GCEventScript::getHealth_( Simian::DelegateParameter& param )
    {
        ScriptFile<GCEventScript>::CallParameter* p;
        param.As(p);
        sq_pushinteger(p->VM, (attributes_ ? attributes_->Health() : 0));
        p->ReturnValue = true;

        //SVerbose("Health is: " << (attributes_ ? attributes_->Health() : 0));
    }

    void GCEventScript::getTotalHealth_( Simian::DelegateParameter& param )
    {
        ScriptFile<GCEventScript>::CallParameter* p;
        param.As(p);
        sq_pushinteger(p->VM, (attributes_ ? attributes_->MaxHealth() : 0));
        p->ReturnValue = true;

        //SVerbose("Total Health is: " << (attributes_ ? attributes_->MaxHealth() : 0));
    }

    void GCEventScript::cinematicSkipCallback_( Simian::DelegateParameter& param )
    {
        ScriptFile<GCEventScript>::CallParameter* p;
        param.As(p);

        // closure should be on the top of the stack
        const SQChar* name;
        sq_getstring(p->VM, -1, &name);
        skipCallback_ = name;
    }

    void GCEventScript::getOSTicks_( Simian::DelegateParameter& param )
    {
        ScriptFile<GCEventScript>::CallParameter* p;
        param.As(p);
        sq_pushfloat(p->VM, Simian::OS::Instance().GetOSTicks());
        p->ReturnValue = true;
    }

    void GCEventScript::setHealth_( Simian::DelegateParameter& )
    {
        if (!attributes_)
            return;

        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::s32 hp;
        PopStack(hp, stack);

        attributes_->Health(hp);
    }

    void GCEventScript::setForceSeek_( Simian::DelegateParameter& )
    {
        if (!attributes_)
            return;

        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        bool forceSeek;
        PopStack(forceSeek, stack);

        attributes_->ForceSeek(forceSeek);
    }

    void GCEventScript::setAIPassive_( Simian::DelegateParameter& )
    {
        Simian::Entity* entity = ParentEntity();
        GCAIGeneric* ai = 0;
        entity->ComponentByType(GC_AIGENERIC, ai);
        if (!ai)
            return;

        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        bool aiPassive;
        PopStack(aiPassive, stack);

        ai->AiPassive(aiPassive);
    }

    void GCEventScript::setSeekInCine_( Simian::DelegateParameter& )
    {
        Simian::Entity* entity = ParentEntity();
        GCAIGeneric* ai = 0;
        entity->ComponentByType(GC_AIGENERIC, ai);
        if (!ai)
            return;

        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        bool seekInCine;
        PopStack(seekInCine, stack);

        ai->SeekInCine(seekInCine);
    }

    void GCEventScript::wait_(Simian::DelegateParameter& param)
    {
        Parameter* p;
        param.As(p);
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        if (!waitTimer_)
        {
            PopStack(waitTimer_, stack);
        }
        else
        {
            waitTimer_ -= p->Dt;
            if (waitTimer_ <= 0)
            {
                waitTimer_ = 0;
                p->Finished = true;
                return;
            }
        }

        p->Finished = false;
    }

    void GCEventScript::playAnimation_( Simian::DelegateParameter& param )
    {
        if (!model_ || !model_)
            return;

        if (!&model_->Controller())
            return;

        Parameter* p;
        param.As(p);
        char* stack = GetStackPtr();
        
        Simian::s32 args;
        PopStack(args, stack);

        std::string animationName;
        PopStack(animationName, stack);
        args--;

        bool loop = true;
        if (args-- > 0)
            PopStack(loop, stack);

        Simian::s32 fps = -1;
        if (args-- > 0)
            PopStack(fps, stack);

        model_->Controller().PlayAnimation(animationName, loop, fps);
    }

    void GCEventScript::trigger_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();

        Simian::s32 args;
        PopStack(args, stack);

        std::string listener;
        PopStack(listener, stack);

        --args;

        std::string target = "";
        if (args-- > 0)
            PopStack(target, stack);

        Simian::Entity* entity = ParentEntity();
        if (target != "")
            entity = ParentScene()->EntityByNameRecursive(target);
        else
            target = ParentEntity()->Name();

        if (!entity)
        {
            SWarn("Cannot trigger entity: \"" << target << "\", not found.");
            return;
        }

        Simian::u32 componentType;
        if (!Simian::GameFactory::Instance().ComponentTable().HasValue(listener))
        {
            SWarn("No such component: " << listener << ".")
            return;
        }
        componentType = Simian::GameFactory::Instance().ComponentTable().Value(listener);

        GCInputListener* component;
        entity->ComponentByType(componentType, component);

        if (!component)
        {
            SWarn("Listener \"" << listener << "\" not found in entity: " << target << ".");
            return;
        }

        GCInputListener::Parameter inputParam;
        inputParam.Callee = ParentEntity();
        component->OnTriggered(inputParam);
    }

    void GCEventScript::scriptSetGlobal_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* entity;
        PopStack(entity, stack);

        if (!entity)
            return;

        GCEventScript* script;
        entity->ComponentByType(GC_EVENTSCRIPT, script);

        if (!script)
        {
            SWarn("Entity: " << entity->Name() << " does not have an event script to set global for.");
            return;
        }

        std::string variable, value;
        PopStack(variable, stack);
        PopStack(value, stack);
        // add the variable and value
        script->AddGlobal(variable, value);
    }

    void GCEventScript::setEnabled_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        bool enabled;
        PopStack(enabled, stack);

        disabled_ = !enabled;
    }

    void GCEventScript::setSkip_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        bool skip;
        PopStack(skip, stack);
        skipped_ = !skip;

        // hide/show the skip text ui thing
        GCCinematicUI::Instance().SkipTextVisible(skip);
    }

    void GCEventScript::jukeboxPlay_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* jukeboxEntity;
        PopStack(jukeboxEntity, stack);

        Simian::s32 trackId;
        PopStack(trackId, stack);

        Simian::CJukebox* jukebox;
        jukeboxEntity->ComponentByType(Simian::C_JUKEBOX, jukebox);
        jukebox->Play(trackId);
    }

    void GCEventScript::jukeboxPlayNext_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* jukeboxEntity;
        PopStack(jukeboxEntity, stack);

        Simian::CJukebox* jukebox;
        jukeboxEntity->ComponentByType(Simian::C_JUKEBOX, jukebox);
        jukebox->PlayNext();
    }

    void GCEventScript::jukeboxCrossfade_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* jukeboxEntity;
        PopStack(jukeboxEntity, stack);

        Simian::s32 trackId;
        PopStack(trackId, stack);

        Simian::f32 time;
        PopStack(time, stack);

        Simian::CJukebox* jukebox;
        jukeboxEntity->ComponentByType(Simian::C_JUKEBOX, jukebox);
        jukebox->CrossFade(trackId, time);
    }

    void GCEventScript::soundEmitterPlay_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* soundEntity;
        PopStack(soundEntity, stack);

        Simian::s32 soundId;
        PopStack(soundId, stack);

        Simian::CSoundEmitter* soundEmitter;
        soundEntity->ComponentByType(Simian::C_SOUNDEMITTER, soundEmitter);

        if (!soundEmitter)
        {
            SWarn("Entity \"" << soundEntity->Name() << "\" has no sound emitter.");
            return;
        }

        soundEmitter->Play(soundId);
    }

    void GCEventScript::soundEmitterStopAll_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* soundEntity;
        PopStack(soundEntity, stack);

        Simian::CSoundEmitter* soundEmitter;
        soundEntity->ComponentByType(Simian::C_SOUNDEMITTER, soundEmitter);

        if (!soundEmitter)
        {
            SWarn("Entity \"" << soundEntity->Name() << "\" has no sound emitter.");
            return;
        }

        soundEmitter->StopAll();
    }

    void GCEventScript::particleSystemSetEnabled_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* entity;
        PopStack(entity, stack);

        bool enabled;
        PopStack(enabled, stack);

        Simian::CParticleSystem* particleSystem_;
        entity->ComponentByType(Simian::C_PARTICLESYSTEM, particleSystem_);
        
        particleSystem_->ParticleSystem()->Enabled(enabled);
    }

    void GCEventScript::cameraInterpolate_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        
        Simian::s32 args;
        PopStack(args, stack);

        Simian::f32 x, y, z;
        PopStack(x, stack);
        PopStack(y, stack);
        PopStack(z, stack);
         
        Simian::f32 time;
        PopStack(time, stack);

        args -= 4;

        std::string interpolationType = "Linear";
        if (args-- > 0)
            PopStack(interpolationType, stack);
        Simian::f32(*interpolator)(Simian::f32) = Simian::Interpolation::Linear;
        interpretInterpolator_(interpolator, interpolationType);

        Simian::Vector3 yawPitchZoom(GCDescensionCamera::Instance()->YawPitchZoom());
        Simian::Vector3 newYawPitchZoom(yawPitchZoom);

        if (args-- > 0)
        {
            Simian::f32 yaw;
            PopStack(yaw, stack);
            newYawPitchZoom.X(yaw);
        }
        if (args-- > 0)
        {
            Simian::f32 pitch;
            PopStack(pitch, stack);
            newYawPitchZoom.Y(pitch);
        }
        if (args-- > 0)
        {
            Simian::f32 zoom;
            PopStack(zoom, stack);
            newYawPitchZoom.Z(zoom);
        }

        // find the difference
        yawPitchZoom = newYawPitchZoom - yawPitchZoom;

        GCDescensionCamera::Instance()->Interpolate(Simian::Vector3(x, y, z),
            yawPitchZoom, time, interpolator);
    }

    void GCEventScript::cinematicFadeOut_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();

        Simian::s32 args;
        PopStack(args, stack);

        Simian::f32 r, g, b, a, duration, depth = 0.0f;
        PopStack(r, stack);
        PopStack(g, stack);
        PopStack(b, stack);
        PopStack(a, stack);
        PopStack(duration, stack);

        if (args - 5 > 0)
            PopStack(depth, stack);

		//if (GCPlayerLogic::Instance())
            //GCPlayerLogic::Instance()->PlayerControlOverride(true);
        GCCinematicUI::Instance().FadeOut(Simian::Color(r, g, b, a), duration, depth);
    }

    void GCEventScript::cinematicShowBorder_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();

        Simian::s32 args;
        PopStack(args, stack);

        Simian::f32 duration;
        PopStack(duration, stack);
        
        bool skipable = true;
        if (--args > 0)
            PopStack(skipable, stack);

        // hide the spirit of demae
        Simian::Entity* entity = ParentScene()->EntityByTypeRecursive(E_SPIRITOFDEMAE);
        if (entity)
        {
            GCMousePlaneIntersection* spirit;
            entity->ComponentByType(GC_MOUSEPLANEINTERSECTION, spirit);
            spirit->Active(false);
        }

        GCCinematicUI::Instance().ShowCinematicBorders(duration, skipable);
    }

    void GCEventScript::cinematicHideBorder_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::f32 duration;
        PopStack(duration, stack);

        // show the spirit of demae
        Simian::Entity* entity = ParentScene()->EntityByTypeRecursive(E_SPIRITOFDEMAE);
        if (entity)
        {
            GCMousePlaneIntersection* spirit;
            entity->ComponentByType(GC_MOUSEPLANEINTERSECTION, spirit);
            spirit->Active(true);
        }

        GCCinematicUI::Instance().HideCinematicBorders(duration);
    }

    void GCEventScript::cinematicShowSubtitle_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        std::string subtitle;
        PopStack(subtitle, stack);

        Simian::f32 duration;
        PopStack(duration, stack);

        GCCinematicUI::Instance().ShowSubtitle(subtitle, duration);
    }

    void GCEventScript::cinematicShakeScreen_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::f32 amount, duration;
        PopStack(amount, stack);
        PopStack(duration, stack);

        GCDescensionCamera::Instance()->Shake(amount, duration);
    }

    void GCEventScript::spriteSetVisible_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* entity;
        PopStack(entity, stack);

        bool visible;
        PopStack(visible, stack);

        Simian::CSprite* sprite;
        entity->ComponentByType(Simian::C_SPRITE, sprite);
        sprite->Visible(visible);
    }

    void GCEventScript::entityCreate_( Simian::DelegateParameter& param )
    {
        // creates a new entity immediately, returning it
        ScriptFile<GCEventScript>::CallParameter* p;
        param.As(p);

        const SQChar* s;
        sq_getstring(p->VM, 2, &s);
        
        // try to create an entity
        if (!Simian::GameFactory::Instance().EntityTable().HasValue(s))
        {
            SWarn("Failed to create entity: \"" << s << "\", doesn't exist.");
            return;
        }

        Simian::s32 type = Simian::GameFactory::Instance().EntityTable().Value(s);
        Simian::Entity* entity = ParentScene()->CreateEntity(type);

        entities_.push_back(entity);

        sq_pushuserpointer(p->VM, (SQUserPointer)entity);
        p->ReturnValue = true;
    }

    void GCEventScript::entityAdd_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        
        Simian::s32 args;
        PopStack(args, stack);

        Simian::Entity* entity;
        PopStack(entity, stack);

        if (--args > 0)
        {
            Simian::Entity* parent;
            PopStack(parent, stack);
            parent->AddChild(entity);
        }
        else
        {
            // add the entity
            ParentScene()->AddEntity(entity);
        }

        std::vector<Simian::Entity*>::iterator i = std::find(entities_.begin(), entities_.end(), entity);
        if (i != entities_.end())
            entities_.erase(i);
    }

    void GCEventScript::entityRemove_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* entity;
        PopStack(entity, stack);

        // remove the entity
        if (entity->ParentEntity())
            entity->ParentEntity()->RemoveChild(entity);
        else
            ParentScene()->RemoveEntity(entity);
    }

    void GCEventScript::entitySetPosition_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* entity;
        PopStack(entity, stack);

        Simian::f32 x, y, z;
        PopStack(x, stack);
        PopStack(y, stack);
        PopStack(z, stack);

        Simian::CTransform* transform;
        entity->ComponentByType(Simian::C_TRANSFORM, transform);

        if (!transform)
        {
            SWarn("Entity \"" << entity->Name() << "\" has no transform component!");
            return;
        }

        Simian::Vector3 pos(x, y, z);
        // world to local transform
        if (transform->ParentTransform())
            transform->ParentTransform()->World().Inversed().Transform(pos);

        // set the position
        transform->Position(pos);
    }

    void GCEventScript::entitySetPositionAtEntity_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* entity;
        PopStack(entity, stack);

        Simian::Entity* destination;
        PopStack(destination, stack);

        Simian::CTransform* transform;
        entity->ComponentByType(Simian::C_TRANSFORM, transform);

        Simian::CTransform* destTransform;
        destination->ComponentByType(Simian::C_TRANSFORM, destTransform);

        if (!transform || !destTransform)
        {
            SWarn("Entity \"" << entity->Name() << "\" or \"" << destination->Name() << "\" has no transform component!");
            return;
        }

        Simian::Vector3 pos(destTransform->World().Position());
        // world to local transform
        if (transform->ParentTransform())
            transform->ParentTransform()->World().Inversed().Transform(pos);

        // set the position
        transform->Position(pos);
    }

    void GCEventScript::entitySetPositionAtEntityYOffset_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* entity;
        PopStack(entity, stack);

        Simian::Entity* destination;
        PopStack(destination, stack);

        Simian::f32 yOffset;
        PopStack(yOffset, stack);

        Simian::CTransform* transform;
        entity->ComponentByType(Simian::C_TRANSFORM, transform);

        Simian::CTransform* destTransform;
        destination->ComponentByType(Simian::C_TRANSFORM, destTransform);

        if (!transform || !destTransform)
        {
            SWarn("Entity \"" << entity->Name() << "\" or \"" << destination->Name() << "\" has no transform component!");
            return;
        }

        Simian::Vector3 pos(destTransform->World().Position());
        // world to local transform
        if (transform->ParentTransform())
            transform->ParentTransform()->World().Inversed().Transform(pos);

        // set the position
        transform->Position(pos + Simian::Vector3(0, yOffset, 0));
    }

    void GCEventScript::entitySetRotation_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* entity;
        PopStack(entity, stack);

        Simian::f32 x, y, z, angle;
        PopStack(x, stack);
        PopStack(y, stack);
        PopStack(z, stack);
        PopStack(angle, stack);

        Simian::CTransform* transform;
        entity->ComponentByType(Simian::C_TRANSFORM, transform);

        if (!transform)
        {
            SWarn("Entity \"" << entity->Name() << "\" doesn't have a transform.");
            return;
        }

        transform->Rotation(Simian::Vector3(x, y, z), Simian::Degree(angle));
    }

    void GCEventScript::entityPlayAnimation_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();

        Simian::s32 args;
        PopStack(args, stack);

        Simian::Entity* entity;
        PopStack(entity, stack);

        if (!entity)
            return;

        Simian::CModel* model;
        entity->ComponentByType(Simian::C_MODEL, model);

        if (!model)
            return;

        if (!&model->Controller())
            return;

        std::string animationName;
        PopStack(animationName, stack);

        args -= 2;

        // get loopness
        bool loop = true;
        if (args-- > 0)
            PopStack(loop, stack);

        Simian::s32 fps = -1;
        if (args-- > 0)
            PopStack(fps, stack);

        model->Controller().PlayAnimation(animationName, loop, fps);
    }

    void GCEventScript::entityModelSetVisible_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* entity;
        PopStack(entity, stack);

        if (!entity)
            return;

        bool visible;
        PopStack(visible, stack);

        Simian::CModel* model;
        entity->ComponentByType(Simian::C_MODEL, model);

        if (!model)
        {
            SWarn("Entity: " << entity->Name() << " does not contain a model to set visibility.");
            return;
        }

        model->Visible(visible);
    }

    void GCEventScript::cinematicMotionTranslate_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* entity;
        PopStack(entity, stack);

        if (!entity)
            return;

        Simian::f32 x, y, z;
        PopStack(x, stack);
        PopStack(y, stack);
        PopStack(z, stack);

        Simian::f32 duration;
        PopStack(duration, stack);

        GCCinematicMotion* motion;
        entity->ComponentByType(GC_CINEMATICMOTION, motion);

        if (!motion)
        {
            SWarn("Entity: " << entity->Name() << " requires a CinematicMotion component.");
            return;
        }

        motion->Translate(Simian::Vector3(x, y, z), duration);
    }

    void GCEventScript::cinematicMotionRotate_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* entity;
        PopStack(entity, stack);

        if (!entity)
            return;

        Simian::f32 angle;
        PopStack(angle, stack);

        Simian::f32 duration;
        PopStack(duration, stack);

        GCCinematicMotion* motion;
        entity->ComponentByType(GC_CINEMATICMOTION, motion);

        if (!motion)
        {
            SWarn("Entity: " << entity->Name() << " requires a CinematicMotion component.");
            return;
        }

        motion->Rotate(Simian::Degree(angle), duration);
    }

    void GCEventScript::entityDealDamage_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::Entity* entity;
        PopStack(entity, stack);

        GCAttributes* attribs = 0;
        entity->ComponentByType(GC_ATTRIBUTES, attribs);

        if (!attribs)
        {
            SWarn("Entity: " << entity->Name() << " has no attributes.");
            return;
        }

        Simian::s32 amount;
        PopStack(amount, stack);

        CombatCommandSystem::Instance().AddCommand(0, attribs, CombatCommandDealDamage::Create(amount));
    }

    void GCEventScript::playerLockCamera_( Simian::DelegateParameter& )
    {
        // lock camera to player
        char* stack = GetStackPtr();

        Simian::s32 args;
        PopStack(args, stack);

        bool lock;
        PopStack(lock, stack);

        if (--args > 0)
        {
            Simian::f32 duration;
            PopStack(duration, stack);
            Simian::Vector3 yawPitchZoom = GCDescensionCamera::Instance()->StartingYawPitchZoom() - GCDescensionCamera::Instance()->YawPitchZoom();

            std::string interpolatorType = "Linear";
            if (args-- > 0)
                PopStack(interpolatorType, stack);
            Simian::f32 (*interpolator)(Simian::f32);
            interpretInterpolator_(interpolator, interpolatorType);

            GCDescensionCamera::Instance()->Interpolate(GCPlayerLogic::Instance()->Transform().World().Position(),
                yawPitchZoom, duration, interpolator);
        }

        GCPlayerLogic::Instance()->CameraLocked(lock);
    }

    void GCEventScript::playerLockMovement_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        bool lock;
        PopStack(lock, stack);

        GCPlayerLogic::Instance()->PlayerControlOverride(lock);
    }

    void GCEventScript::playerRegainMaxHp_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        bool regain;
        PopStack(regain, stack);

        if (regain)
        {
            CombatCommandSystem::Instance().AddCommand(0, GCPlayerLogic::Instance()->Attributes(),
                CombatCommandHeal::Create(GCPlayerLogic::Instance()->Attributes()->MaxHealth(),
                                          GCPlayerLogic::Instance()->Attributes()->MaxMana()));
            GCPlayerLogic::Instance()->ResetHeartBeatAndDyingEffect();
        }
    }

    void GCEventScript::playerPlayTeleport_( Simian::DelegateParameter& )
    {
        GCPlayerLogic::Instance()->PlayTeleportingEffect(true);
    }

    void GCEventScript::playerStopTeleport_( Simian::DelegateParameter& )
    {
        GCPlayerLogic::Instance()->StopTeleportingEffect();
    }

    void GCEventScript::playerSpiritSwirlPlay_( Simian::DelegateParameter& )
    {
        GCPlayerLogic::Instance()->PlaySpiritSwirl();
    }

    void GCEventScript::playerSpiritSwirlStop_( Simian::DelegateParameter& )
    {
        GCPlayerLogic::Instance()->StopSpiritSwirl();
    }

    void GCEventScript::playerAddExperience_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();

        stack += sizeof(Simian::s32);

        Simian::s32 exp;
        PopStack(exp, stack);

        GCPlayerLogic::Instance()->AddExperience(exp);
    }

    void GCEventScript::particleTimelinePlay_(Simian::DelegateParameter&)
	{
		char* stack = GetStackPtr();

		stack += sizeof(Simian::s32);

		Simian::Entity* entity;
		PopStack(entity, stack);

		Simian::CParticleTimeline* timeline;
		entity->ComponentByType(Simian::C_PARTICLETIMELINE, timeline);

		if (!timeline)
		{
			SWarn("Entity: " << entity->Name() << " does not have particle timeline.");
			return;
		}

		timeline->Play();
	}

	void GCEventScript::particleTimelineStop_(Simian::DelegateParameter&)
	{
		char* stack = GetStackPtr();

		stack += sizeof(Simian::s32);

		Simian::Entity* entity;
		PopStack(entity, stack);

		Simian::CParticleTimeline* timeline;
		entity->ComponentByType(Simian::C_PARTICLETIMELINE, timeline);

		if (!timeline)
		{
			SWarn("Entity: " << entity->Name() << " does not have particle timeline.");
			return;
		}

		timeline->Stop();
	}

    void GCEventScript::trapEnable_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();

        Simian::s32 args;
        PopStack(args, stack);

        Simian::Entity* entity;
        PopStack(entity, stack);

        if (!entity)
            return;

        GCTrapLaserBeam* trap1 = 0;
        GCTrapFireSpot* trap2 = 0;
        entity->ComponentByType(GC_TRAPLASERBEAM, trap1);
        entity->ComponentByType(GC_TRAPFIRESPOT, trap2);

        bool enable_;
        PopStack(enable_, stack);

        if (trap1)
            trap1->Enabled(enable_);
        else
            trap2->Enabled(enable_);
    }

    void GCEventScript::wallStartDescend_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();

        Simian::s32 args;
        PopStack(args, stack);

        Simian::Entity* entity;
        PopStack(entity, stack);

        if (!entity)
            return;

        GCDescendingWall* wall = 0;
        entity->ComponentByType(GC_DESCENDINGWALL, wall);

        bool startDescend;
        PopStack(startDescend, stack);

        if (wall)
            wall->StartDescend(startDescend);
    }

    void GCEventScript::pathfinderContinuePathfinding_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        bool continueFinding;
        PopStack(continueFinding, stack);

        PathFinder::Instance().ContinuePathfinding(continueFinding);
    }

    void GCEventScript::puzzleLeverTrigger_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::u32 puzzleID;
        PopStack(puzzleID, stack);
        GCGestureInterpreter::Instance()->PuzzleModeID(puzzleID);
    }

    void GCEventScript::gestureUnlock_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::u32 gestureID;
        PopStack(gestureID, stack);
        GCGestureInterpreter::Instance()->SetSkills(
            GestureSkills( GCGestureInterpreter::Instance()->GetSkills().Skills()
            | (1<<gestureID) ));
    }

    void GCEventScript::enablePhysics_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        std::string entityName;
        PopStack(entityName, stack);

        Simian::Entity* entity = ParentScene()->EntityByNameRecursive(entityName);
        if (!entity)
        {
            SWarn("Could not find entity: \"" << entityName << "\"");
            return;
        }

        GCPhysicsController* physics;
        entity->ComponentByType(GC_PHYSICSCONTROLLER, physics);
        physics->Enabled(true);
    }

    void GCEventScript::disablePhysics_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        std::string entityName;
        PopStack(entityName, stack);

        Simian::Entity* entity = ParentScene()->EntityByNameRecursive(entityName);
        if (!entity)
        {
            SWarn("Could not find entity: \"" << entityName << "\"");
            return;
        }

        GCPhysicsController* physics;
        entity->ComponentByType(GC_PHYSICSCONTROLLER, physics);
        physics->Enabled(false);
    }

    void GCEventScript::canEscapeCinematic_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        Simian::u32 val;
        PopStack(val, stack);
        GCCinematicEscape::CanEscape(val ? true : false);
    }

    void GCEventScript::displayName_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        std::string bossName;
        Simian::f32 inTime, holdTime, outTime;

        PopStack(inTime, stack);
        PopStack(holdTime, stack);
        PopStack(outTime, stack);
        PopStack(bossName, stack);

        if (bossName == "CrimsonRevenant")
            GCUI::Instance()->ShowBossName(inTime, holdTime, outTime,
            GCUI::UI_CRIMSON_REVENANT);
        else if (bossName == "Vulcan")
            GCUI::Instance()->ShowBossName(inTime, holdTime, outTime,
            GCUI::UI_VULCAN);
        else if (bossName == "CrystalGuardian")
            GCUI::Instance()->ShowBossName(inTime, holdTime, outTime,
            GCUI::UI_CRYSTAL_GUARDIAN);
        else
        {
            //--Expecting number now
            std::stringstream sstr;
            sstr << bossName;

            Simian::u32 bossID;
            sstr >> bossID;

            GCUI::Instance()->ShowBossName(inTime, holdTime, outTime,
                GCUI::UI_A0+bossID);
        }
    }

    void GCEventScript::drawGesture_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();
        stack += sizeof(Simian::s32);

        std::string gType;
        PopStack(gType, stack);

        if (gType == "Slash")
            GestureTutor::DrawType(GestureTutor::GT_SLASH);
        else if (gType == "Lightning")
            GestureTutor::DrawType(GestureTutor::GT_LIGHTNING);
        else if (gType == "Fire")
            GestureTutor::DrawType(GestureTutor::GT_FIRE);
        else if (gType == "None")
            GestureTutor::DrawType(GestureTutor::GT_TOTAL);
        else
            SWarn("!!!GestureType is invalid!!!");
    }

    void GCEventScript::saveGameData_( Simian::DelegateParameter& )
    {
        SaveManager& saveManager = SaveManager::Instance();
        saveManager.UpdateCurrentData();

        Simian::s32 argc;
        char* stack = GetStackPtr();
        PopStack(argc, stack);

        bool usePosVec = false;
        if (argc > 0)
        {
            --argc;
            PopStack(usePosVec, stack);
        }

        Simian::f32 x=0, z=0;
        if (argc > 1)
        {
            --argc;
            PopStack(x, stack);
            --argc;
            PopStack(z, stack);
        }

        Simian::u32 saveID = 0;

        if (usePosVec) //--use custom param
        {
            saveManager.CurrentData().Position =
                Simian::Vector2(x, z);
            saveID = SaveManager::SAVE_CHECKPOINT;
        } else { //--use current player position
            saveManager.CurrentData().Position = Simian::Vector2
               (GCPlayerLogic::Instance()->Physics()->Position().X(),
                GCPlayerLogic::Instance()->Physics()->Position().Z() );
            saveID = SaveManager::SAVE_NORMAL;
        }

        if (argc == 1)
        {
            std::string nextLv;

            PopStack(nextLv, stack);
            saveManager.SetCurrentLevel(nextLv, saveManager.CurrentData());
        }

        saveManager.SaveData(saveID);
    }

    void GCEventScript::tilemapSetWall_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();

        Simian::s32 args;
        PopStack(args, stack);

        bool state;
        PopStack(state, stack);

        Simian::s32 x, y;
        PopStack(x, stack);
        PopStack(y, stack);

        std::string direction;
        PopStack(direction, stack);

        args -= 4;
        
        bool doubleSided = true;
        if (args-- > 0)
            PopStack(doubleSided, stack);

        Simian::s32 otherX = x;
        Simian::s32 otherY = y;
        Tile::TILE_FLAG wall = Tile::TILE_FLOOR;
        Tile::TILE_FLAG otherWall = Tile::TILE_FLOOR;
        if (direction == "NORTH")
        {
            wall = Tile::TILE_NORTH;
            otherWall = Tile::TILE_SOUTH;
            otherY += 1;
        }
        else if (direction == "SOUTH")
        {
            wall = Tile::TILE_SOUTH;
            otherWall = Tile::TILE_NORTH;
            otherY += -1;
        }
        else if (direction == "EAST")
        {
            wall = Tile::TILE_EAST;
            otherWall = Tile::TILE_WEST;
            otherX += 1;
        }
        else if (direction == "WEST")
        {
            wall = Tile::TILE_WEST;
            otherWall = Tile::TILE_EAST;
            otherX += -1;
        }
        else if (direction == "DIAGDOWN")
        {
            wall = Tile::TILE_DIAGDOWN;
            otherWall = Tile::TILE_DIAGDOWN;
        }
        else if (direction == "DIAGUP")
        {
            wall = Tile::TILE_DIAGUP;
            otherWall = Tile::TILE_DIAGUP;
        }
        else 
        {
            SWarn("No such direction: " << direction << ".");
            return;
        }

        // disable the walls in both ai and tile map
        TileMap& tileMap = GCTileMap::TileMap(ParentScene())->TileMap();
        SFlagSet(tileMap[x][y].Flag, wall, state);
        PathFinder::Instance().SetTileEdge(state, x, y, wall);
        
        if (otherX >= 0 && otherX < (Simian::s32)tileMap.Width() &&
            otherY >= 0 && otherY < (Simian::s32)tileMap.Height())
        {
            SFlagSet(tileMap[otherX][otherY].Flag, otherWall, state);
            PathFinder::Instance().SetTileEdge(state, otherX, otherY, otherWall);
        }
    }

    void GCEventScript::levelChange_( Simian::DelegateParameter& )
    {
        char* stack = GetStackPtr();

        Simian::s32 args;
        PopStack(args, stack);

        // get the next level path
        std::string levelLocation;
        PopStack(levelLocation, stack);

        bool blankLoadScreen = args > 1;

        if (blankLoadScreen)
        {
            StateLoading::Instance().Timer(1.0f);
            StateLoading::Instance().BackgroundColor(Simian::Color(1.0f, 1.0f, 1.0f, 1.0f));
            StateLoading::Instance().HideLoadScreen(true);
        }

        // set the change level state
        StateGame::Instance().LevelFile(levelLocation);
        StateLoading::Instance().NextState(StateGame::InstancePtr());
        Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
    }
}
