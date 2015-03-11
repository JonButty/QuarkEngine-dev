/*************************************************************************/
/*!
\file		GCGesturePuzzle.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/09/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/*************************************************************************/
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/Entity.h"
#include "Simian/Interpolation.h"
#include "Simian/Keyboard.h"
#include "Simian/Mouse.h"
#include "Simian/Scene.h"
#include "Simian/CCamera.h"
#include "Simian/CSprite.h"
#include "Simian/Game.h"
#include "Simian/CTransform.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"

#include "GCUIButton.h"
#include "GCGesturePuzzle.h"
#include "GCGestureInterpreter.h"
#include "GCUI.h"
#include "GCEditor.h"
#include "GCDescensionCamera.h"
#include "GCPlayerLogic.h"
#include "GCCinematicUI.h"
#include "GCEventScript.h"
#include "GCMousePlaneIntersection.h"
#include "GCCursor.h"
#include "GCFadeSpriteEffect.h"
#include "GCPlayerLogic.h"
#include "GCPhysicsController.h"

namespace Descension
{
    GCGesturePuzzle* GCGesturePuzzle::instance_ = NULL;
    Simian::FactoryPlant<Simian::EntityComponent, GCGesturePuzzle> GCGesturePuzzle::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_GESTUREPUZZLE);

    Simian::u32 GCGesturePuzzle::puzzleFlag_ = 0;
    bool GCGesturePuzzle::isActive_ = false;
    bool GCGesturePuzzle::newStart = false;

    GCGesturePuzzle::GCGesturePuzzle()
        : puzzleID_(0), cameraNear_(1.0f), interact_(NULL), backBtn_(NULL),
          timer_(1001.0f),
          fsm_(GPUZZLE_IDLE, GPUZZLE_TOTALSTATES)
    {  }

    //--------------------------------------------------------------------------
    Simian::u32 GCGesturePuzzle::PuzzleID() const
    {
        return puzzleID_;
    }
    void GCGesturePuzzle::UnlockPuzzle(Simian::u32 id)
    {
        SFlagSet(puzzleFlag_, id-1);
    }
    bool GCGesturePuzzle::IsPuzzleUnlocked(Simian::u32 id)
    {
        return SIsFlagSet(puzzleFlag_, id>>1);
    }
    bool GCGesturePuzzle::IsActive(void)
    {
        return isActive_;
    }

    //--------------------------------------------------------------------------
    void GCGesturePuzzle::IntroUpdate_(Simian::DelegateParameter&)
    {
        isActive_ = true;
        fsm_.ChangeState(GPUZZLE_START);
        GCGestureInterpreter::Instance()->PuzzleSolved(); //--clear prev
    }

    void GCGesturePuzzle::StartUpdate_(Simian::DelegateParameter&)
    {
        //GCPlayerLogic::Instance()->Attributes()->Mana(
        //    GCPlayerLogic::Instance()->Attributes()->MaxMana());
        if (GCGestureInterpreter::Instance()->PuzzleSolved())
            fsm_.ChangeState(GPUZZLE_PASSED);
    }

    void GCGesturePuzzle::PassedUpdate_(Simian::DelegateParameter&)
    {
        ExitPuzzle(triggerTarget_);
        fsm_.ChangeState(GPUZZLE_CLOSED);
    }

    void GCGesturePuzzle::ClosedEnter_(Simian::DelegateParameter&)
    {
        SolvedPuzzle();
    }

    void GCGesturePuzzle::SolvedPuzzle()
    {
        GCGestureInterpreter::Instance()->PuzzleModeID(GPUZZLE_NONE);
        SFlagUnset(puzzleID_, GPUZZLE_ENABLED);
        puzzleFlag_ = 0;
        isActive_ = true;
    }

    void GCGesturePuzzle::FailedPuzzle()
    {
        GCGestureInterpreter::Instance()->PuzzleModeID(GPUZZLE_NONE);
    }

    void GCGesturePuzzle::ExitPuzzle(const std::string& trigger)
    {
        Simian::Entity* exiter = ParentScene()->EntityByNameRecursive(trigger);
        if (exiter)
        {
            isActive_ = false;
            GCEventScript* script;
            exiter->ComponentByType(GC_EVENTSCRIPT, script);
            script->OnTriggered(Simian::Delegate::NoParameter);
            GCCursor::Instance()->ShowBoth(false);
            GCCursor::Instance()->Spirit()->Active(true);
        }
        if (backBtn_)
        {
            ParentScene()->RemoveEntity(backBtn_);
            backBtn_ = NULL;
            timer_ = 1001.0f;
        }
    }

    //--------------------------------------------------------------------------
    void GCGesturePuzzle::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if (GCUI::GameStatus() == GCUI::GAME_PAUSED)
            return;

        if (newStart)
        {
            GCFadeSprite* fadeSpriteGC = NULL;
            interact_->ComponentByType(GC_FADESPRITEEFFECT, fadeSpriteGC);
            fadeSpriteGC->ForceEnd();
            newStart = false;
        }

        static const Simian::f32 enterR = 0.75f;
        Simian::Vector3 player = GCPlayerLogic::Instance()->Physics()->Position();

        if ((player - transform_->Position()).LengthSquared() <= enterR &&
            !isActive_)
        {
            GCFadeSprite* fadeSpriteGC = NULL;
            interact_->ComponentByType(GC_FADESPRITEEFFECT, fadeSpriteGC);
            fadeSpriteGC->Reset();
            if ( Simian::Mouse::IsTriggered(Simian::Mouse::KEY_LBUTTON))
            {
                GCEventScript* script;
                ComponentByType(GC_EVENTSCRIPT, script);
                script->OnTriggered(Simian::Delegate::NoParameter);

                fsm_.ChangeState(GPUZZLE_INTRO);
                isActive_ = true;
                if (SIsFlagSet(puzzleID_, GPUZZLE_ENABLED))
                {
                    GCCursor::Instance()->ShowBoth(true);
                    GCCursor::Instance()->Spirit()->Active(false);
                }

                timer_ = 0.0f;
            }
        }

        if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_ESCAPE) &&
           ( GCGestureInterpreter::Instance()->PuzzleModeID() & PuzzleID()) )
           ExitPuzzle(triggerExit_);

        Simian::f32 winHeight = Simian::Game::Instance().Size().Y(),
            scaleHeight = winHeight/1152.0f;
        Simian::CTransform* fadeSprite = NULL;
        interact_->ComponentByType(Simian::C_TRANSFORM, fadeSprite);
        fadeSprite->Position(Simian::Vector3(0, -winHeight*0.375f));
        fadeSprite->Scale(Simian::Vector3(scaleHeight, scaleHeight, scaleHeight));

        dt_ = param.As<Simian::EntityUpdateParameter>().Dt;

        if (timer_ < 1000.0f)
            timer_ += dt_;

        if (timer_ >= 1.75f && timer_ <= 100.0f && backBtn_ == NULL)
        {
            backBtn_ = ParentScene()->CreateEntity(E_BACKBUTTON);
            ParentScene()->AddEntity(backBtn_);

            GCUIButton* button = NULL;
            backBtn_->ComponentByType(GC_UIBUTTON, button);
            button->AddListener(this);
        }

        if (GCGestureInterpreter::Instance()->PuzzleModeID() != GCGesturePuzzle::GPUZZLE_NONE)
            cameraNear_ -= dt_;
        else
            cameraNear_ += dt_*0.1f;

        cameraNear_ = Simian::Math::Clamp(cameraNear_, 0.01f, 1.0f);
        Simian::CCamera* camera;
        Simian::CCamera::SceneCamera(camera);
        camera->Near(cameraNear_);

        //--Unlocking code by ID
        if ( !(puzzleFlag_&puzzleID_) &&
            GCGestureInterpreter::Instance()->PuzzleModeID() & PuzzleID() )
            return;

        fsm_.Update(dt_);
    }

    //--------------------------------------------------------------------------
    void GCGesturePuzzle::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate
            <GCGesturePuzzle, &GCGesturePuzzle::update_>(this));
    }

    void GCGesturePuzzle::OnAwake()
    {
        instance_ = this;
        isActive_ = false;
        newStart = true;
        fsm_[GPUZZLE_INTRO].OnUpdate = Simian::Delegate::CreateDelegate<GCGesturePuzzle,
            &GCGesturePuzzle::IntroUpdate_>(this);
        fsm_[GPUZZLE_START].OnUpdate = Simian::Delegate::CreateDelegate<GCGesturePuzzle,
            &GCGesturePuzzle::StartUpdate_>(this);
        fsm_[GPUZZLE_PASSED].OnUpdate = Simian::Delegate::CreateDelegate<GCGesturePuzzle,
            &GCGesturePuzzle::PassedUpdate_>(this);
        fsm_[GPUZZLE_CLOSED].OnUpdate = Simian::Delegate::CreateDelegate<GCGesturePuzzle,
            &GCGesturePuzzle::ClosedEnter_>(this);
        fsm_.ChangeState(GPUZZLE_IDLE);
    }

    void GCGesturePuzzle::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        ComponentByType(Simian::C_TRANSFORM, transform_);
        puzzleFlag_ = 0;
        SFlagSet(puzzleID_, GPUZZLE_ENABLED);
        interact_ = ParentScene()->CreateEntity(E_GMESSAGE_INTERACT);
        ParentScene()->AddEntity(interact_);
    }

    void GCGesturePuzzle::OnDeinit()
    {
        FailedPuzzle();
        puzzleFlag_ = 0;
    }

    void GCGesturePuzzle::OnTriggered(Simian::DelegateParameter& param)
    {
        GCInputListener::Parameter* p;
        param.As(p);

        if (p->Callee->Name() == "BackButton")
        {
            ExitPuzzle(triggerExit_);
            return;
        }
    }

    void GCGesturePuzzle::Serialize( Simian::FileObjectSubNode& data)
    {
        SFlagUnset(puzzleID_, GPUZZLE_ENABLED);
        data.AddData("PuzzleID", puzzleID_);
        data.AddData("TriggerTarget", triggerTarget_);
        data.AddData("TriggerExit", triggerExit_);
    }

    void GCGesturePuzzle::Deserialize( const Simian::FileObjectSubNode& data)
    {
        Simian::u32 puzzleID = 0;
        data.Data("PuzzleID", puzzleID, puzzleID);
        if (puzzleID)
            SFlagSet(puzzleID_, puzzleID-1);
        data.Data("TriggerTarget", triggerTarget_, triggerTarget_);
        data.Data("TriggerExit", triggerExit_, triggerExit_);
    }
}
