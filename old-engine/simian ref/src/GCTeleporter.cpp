/*************************************************************************/
/*!
\file		GCTeleporter.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/10/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*************************************************************************/
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"
#include "Simian/Utility.h"
#include "Simian/Game.h"
#include "Simian/Delegate.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian/CModel.h"
#include "Simian/LogManager.h"
#include "Simian/CSoundEmitter.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"

#include "GCPhysicsController.h"
#include "GCPlayerLogic.h"
#include "GCTeleporter.h"
#include "GCEditor.h"
#include "GCCinematicUI.h"

#include <iostream>

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCTeleporter>
        GCTeleporter::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_TELEPORTER);
    
    bool GCTeleporter::teleported_ = false;
    static const Simian::f32 ANIM_TIMER_MAX = 0.05f;
    static const Simian::f32 STOP_TELEPORTING_TIMER_MAX = 0.25f;

    GCTeleporter::GCTeleporter()
        : activated_(false), playerTransform_(0), myTransform_(0),
        toDuration_(0.0f), fromDuration_(0.0f), sounds_(0), radius_(0.0f), animTimer_(0.0f),
        fsm_(TELEPORTER_DISABLED, TELEPORTER_STATE_TOTAL)
    {}

    //--------------------------------------------------------------------------
    void GCTeleporter::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if (!activated_)
            return;

        dt_ = param.As<Simian::EntityUpdateParameter>().Dt;
        fsm_.Update(dt_);
    }

    Simian::CTransform* GCTeleporter::Transform(void)
    {
        return myTransform_;
    }

    //--------------------------------------------------------------------------
    void GCTeleporter::DisabledOnUpdate(Simian::DelegateParameter&)
    {
        static const Simian::f32 padding = 2.0f;
        Simian::Vector3 player(playerTransform_->WorldPosition()),
            thisNode(myTransform_->Position());
        if ((player-thisNode).LengthSquared() >= padding*radius_*radius_)
            fsm_.ChangeState(TELEPORTER_ENABLED);
        teleported_ = false;
    }
    void GCTeleporter::EnabledOnUpdate(Simian::DelegateParameter&)
    {
        Simian::Vector3 player(playerTransform_->WorldPosition()),
            thisNode(myTransform_->Position());
        if ((player-thisNode).LengthSquared() <= radius_*radius_)
            fsm_.ChangeState(TELEPORTER_ACTIVE);
    }
    void GCTeleporter::ActiveOnEnter(Simian::DelegateParameter&)
    {
        timer_ = 0.0f;
        animTimer_ = 0.0f;
        GCPlayerLogic::Instance()->PlayerControlOverride(true);
        GCCinematicUI::Instance().ShowCinematicBorders(0.5f, false);
        //--create particles at current location
        Simian::Entity* particles = NULL;
        particles = ParentEntity()->ParentScene()->CreateEntity(E_TELEPORTEFFECT);
        if ( particles )
        {
            ParentScene()->AddEntity(particles);
            Simian::CTransform* transferParticlestransform_;
            particles->ComponentByType(Simian::C_TRANSFORM, transferParticlestransform_);
            transferParticlestransform_->Position(
                GCPlayerLogic::Instance()->Transform().WorldPosition());
        }
        if (sounds_)
            sounds_->Play(2);
    }
    void GCTeleporter::ActiveOnUpdate(Simian::DelegateParameter&)
    {
        if (teleported_)
        {
            fsm_.ChangeState(TELEPORTER_DISABLED);
            return;
        }
        timer_ += dt_;

        //if (timer_ >= toDuration_*0.5f)
        {
            GCPlayerLogic::Instance()->PlayTeleportingEffect(true);
        }

        if (timer_ > toDuration_)
            fsm_.ChangeState(TELEPORTER_ARRIVE);
    }
    void GCTeleporter::ArriveOnEnter(Simian::DelegateParameter&)
    {
        timer_ = 0.0f;
        animTimer_ = 0.0f;
        GCPlayerLogic::Instance()->Physics()->Enabled(false);
        playerTransform_->ParentTransform()->Position(otherNode_->Transform()->Position());
        GCPlayerLogic::Instance()->Physics()->Enabled(true);

        Simian::CModel* model = NULL;
        GCPlayerLogic::Instance()->ComponentByType(Simian::C_MODEL, model);
        model->Visible(false);
        Simian::Entity* ptr = ParentScene()->EntityByNameRecursive("Sword 1");
        ptr->ComponentByType(Simian::C_MODEL, model);
        model->Visible(false);
        ptr = ParentScene()->EntityByNameRecursive("ShadowPlayer");
        ptr->ComponentByType(Simian::C_MODEL, model);
        model->Visible(false);

        teleported_ = true;
    }
    void GCTeleporter::ArriveOnUpdate(Simian::DelegateParameter&)
    {
        timer_ += dt_;

        if (timer_ >= STOP_TELEPORTING_TIMER_MAX/2.0f)
        {
            GCPlayerLogic::Instance()->StopTeleportingEffect();
        }

        if (timer_ > STOP_TELEPORTING_TIMER_MAX)
        {
            animTimer_ += dt_;
            GCPlayerLogic::Instance()->PlayerControlOverride(false);
            GCPlayerLogic::Instance()->TeleportedPlayer();

            if (sounds_)
                sounds_->Play(1);

            if (animTimer_ > ANIM_TIMER_MAX)
            {
                GCPlayerLogic::Instance()->PlayTeleportingEffect(false);
                Simian::CModel* model = NULL;
                GCPlayerLogic::Instance()->ComponentByType(Simian::C_MODEL, model);
                model->Visible(true);
                Simian::Entity* ptr = ParentScene()->EntityByNameRecursive("Sword 1");
                ptr->ComponentByType(Simian::C_MODEL, model);
                model->Visible(true);
                ptr = ParentScene()->EntityByNameRecursive("ShadowPlayer");
                ptr->ComponentByType(Simian::C_MODEL, model);
                model->Visible(true);
                
                fsm_.ChangeState(TELEPORTER_DISABLED);
                GCCinematicUI::Instance().HideCinematicBorders(1.0f);
            }
        }
    }

    //--------------------------------------------------------------------------
    void GCTeleporter::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate
            <GCTeleporter, &GCTeleporter::update_>(this));
    }

    void GCTeleporter::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, myTransform_);

        fsm_[TELEPORTER_DISABLED].OnUpdate = Simian::Delegate::CreateDelegate
            <GCTeleporter, &GCTeleporter::DisabledOnUpdate>(this);
        fsm_[TELEPORTER_ENABLED].OnUpdate = Simian::Delegate::CreateDelegate
            <GCTeleporter, &GCTeleporter::EnabledOnUpdate>(this);
        fsm_[TELEPORTER_ACTIVE].OnEnter = Simian::Delegate::CreateDelegate
            <GCTeleporter, &GCTeleporter::ActiveOnEnter>(this);
        fsm_[TELEPORTER_ACTIVE].OnUpdate = Simian::Delegate::CreateDelegate
            <GCTeleporter, &GCTeleporter::ActiveOnUpdate>(this);
        fsm_[TELEPORTER_ARRIVE].OnEnter = Simian::Delegate::CreateDelegate
            <GCTeleporter, &GCTeleporter::ArriveOnEnter>(this);
        fsm_[TELEPORTER_ARRIVE].OnUpdate = Simian::Delegate::CreateDelegate
            <GCTeleporter, &GCTeleporter::ArriveOnUpdate>(this);
        fsm_.ChangeState(TELEPORTER_DISABLED);
    }

    void GCTeleporter::OnInit()
    {
        GCPlayerLogic::Instance()->ComponentByType(Simian::C_TRANSFORM,
            playerTransform_);
        Simian::Entity* ptr = ParentScene()->EntityByNameRecursive(otherNodeName_);
        if (ptr)
        {
            ptr->ComponentByType(GC_TELEPORTER, otherNode_);
        }
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);
    }

    void GCTeleporter::OnDeinit()
    {
        //--
    }

    void GCTeleporter::OnTriggered(Simian::DelegateParameter&)
    {
        activated_ = !activated_;
        
        //--Get particles and play
        Simian::Entity* eps = ParentEntity()->ChildByName("Fire Particles");
        Simian::CParticleSystem* ps = NULL;
        eps->ComponentByType(Simian::C_PARTICLESYSTEM, ps);
        ps->ParticleSystem()->Enabled(!ps->ParticleSystem()->Enabled());

        eps = ParentEntity()->ChildByName("White Light");
        Simian::CModel* mod = NULL;
        eps->ComponentByType(Simian::C_MODEL, mod);
        mod->Visible(!mod->Visible());

        if (sounds_)
            sounds_->Play(0);
    }

    void GCTeleporter::Serialize( Simian::FileObjectSubNode& data)
    {
        data.AddData("ToDuration", toDuration_);
        data.AddData("FromDuration", fromDuration_);
        data.AddData("Radius", radius_);
        data.AddData("OtherNode", otherNodeName_);
    }

    void GCTeleporter::Deserialize( const Simian::FileObjectSubNode& data)
    {
        data.Data("ToDuration", toDuration_, toDuration_);
        data.Data("FromDuration", fromDuration_, fromDuration_);
        data.Data("Radius", radius_, radius_);
        data.Data("OtherNode", otherNodeName_, otherNodeName_);
    }
}
