/*************************************************************************/
/*!
\file		GCUI.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCUI.h"
#include "GCAIBase.h"
#include "GCAttributes.h"
#include "GCPlayerLogic.h"
#include "GCEditor.h"
#include "ComponentTypes.h"
#include "GCSpriteEditor.h"
#include "StateLoading.h"
#include "StateGame.h"
#include "GCOptionsMenu.h"
#include "GCCredits.h"
#include "GCHowToPlay.h"
#include "EntityTypes.h"
#include "GCGestureInterpreter.h"
#include "EntityTypes.h"
#include "GCMousePlaneIntersection.h"
#include "GCUpgradeExchange.h"
#include "SaveManager.h"

#include "Simian/CJukebox.h"
#include "Simian/MaterialCache.h"
#include "Simian/VertexBuffer.h"
#include "Simian/VertexFormat.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/RenderQueue.h"
#include "Simian/IndexBuffer.h"
#include "Simian/Vector2.h"
#include "Simian/Utility.h"
#include "Simian/Game.h"
#include "Simian/Camera.h"
#include "Simian/CCamera.h"
#include "Simian/CTransform.h"
#include "Simian/Math.h"
#include "Simian/Mouse.h"
#include "Simian/Keyboard.h"
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/EntityComponent.h"
#include "Simian/Delegate.h"
#include "Simian/Interpolation.h"
#include "Simian/EntityComponent.h"
#include "Simian/Scene.h"
#include "Simian/LogManager.h"
#include "Simian/ParticleSystem.h"
#include "Simian/CParticleSystem.h"
#include "Simian/CUIResize.h"

#include <algorithm>

namespace Descension
{
    std::vector<std::vector<Simian::Vector2> > GCUI::uiFrames;
    Simian::u32 GCUI::gameStatus_ = GCUI::GAME_RUNNING, GCUI::bossID_ = 0;
    GCAIBase* GCUI::bossPtr_ = NULL;
    GCUI* GCUI::pGCUI = NULL;

    const Simian::u32 GCUI::MAX_SPRITE_COUNT = 64,
                      GCUI::POLY_VERTEX_COUNT = 4, //quads
                      GCUI::MAX_VBSIZE = MAX_SPRITE_COUNT*POLY_VERTEX_COUNT,
                      GCUI::ALPHABIT = 24;
    const Simian::f32 GCUI::UV_EPSILON = 0.001f,
                      GCUI::ICON_START_SCALE = 4.0f,
                      GCUI::BAR_RELSCALE_X = 1.02f,
                      GCUI::BAR_RELSCALE_Y = 1.4f;
    Simian::FactoryPlant<Simian::EntityComponent, GCUI>
        GCUI::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_UI);
    //--------------------------------------------------------------------------
    void GCUI::UpdateUI_(void)
    {
        if (cinematics_)
        {
            if (GCPlayerLogic::Instance()->PlayerControlOverride() != cinematics_)
            {
                cinematics_ = GCPlayerLogic::Instance()->PlayerControlOverride();
                LoadUI_();
                LoadSkills();
                UnloadSkills(true);
                LoadSkills();
            }
            return;
        }
        if (GCPlayerLogic::Instance()->PlayerControlOverride()) //--Unload UIBars
        {
            cinematics_ = true;
            UnloadUI_();
            UnloadSkills();
            return;
        }

        const Simian::f32 flashTime = 0.1f;
        GCAttributes* pAttribs = GCPlayerLogic::Instance()->Attributes();
        static Simian::s32 prevHP = pAttribs->Health();
        static Simian::f32 prevMP = pAttribs->Mana();
        static Simian::s32 prevMaxHP = pAttribs->MinMaxHealth();
        static Simian::f32 prevMaxMP = pAttribs->MinMaxMana();
        static Simian::u32 prevMaxExP = pAttribs->MinExperienceGoal();
        static Simian::u32 prevEXP = 0;
        static Simian::f32 hpFlash = flashTime;

        if (checkBars_)
        {
            prevHP = pAttribs->Health();
            prevMP = pAttribs->Mana();
            prevMaxHP = pAttribs->MinMaxHealth();
            prevMaxMP = pAttribs->MinMaxMana();
            prevMaxExP = pAttribs->MinExperienceGoal();
            prevEXP = 0;
            hpFlash = flashTime;
        }

        if (pAttribs->Health() != prevHP || pAttribs->Experience() != prevEXP ||
            pAttribs->ExperienceGoal() != prevMaxExP ||
            pAttribs->Experience() != prevEXP ||
            pAttribs->MaxHealth() != prevMaxHP || pAttribs->MaxMana() != prevMaxMP ||
            std::abs(pAttribs->Mana() - prevMP) > Simian::Math::EPSILON)
        {
            Simian::Vector2 winSize(Simian::Game::Instance().Size());
            Sprite* pHP = GetSpritePtr("HP");
            //flash effect
            if (pAttribs->Health() != prevHP && pAttribs->Health() > 0
                && pAttribs->Health() < prevHP)
            {
                pHP->UV(&uiFrames[UI_FLASH]);
                hpFlash = flashTime;
            }
            pHP->Scale(Simian::Vector2(static_cast<Simian::f32>(
                pAttribs->Health())/pAttribs->MinMaxHealth(), 1.0f));
            GetSprite("MP").Scale(Simian::Vector2(static_cast<Simian::f32>(
                pAttribs->Mana())/pAttribs->MinMaxMana(), 1.0f));
            GetSprite("HPBar").Scale(Simian::Vector2(static_cast<Simian::f32>(
                pAttribs->MaxHealth())/pAttribs->MinMaxHealth()
                *BAR_RELSCALE_X, BAR_RELSCALE_Y));
            GetSprite("MPBar").Scale(Simian::Vector2(static_cast<Simian::f32>(
                pAttribs->MaxMana())/pAttribs->MinMaxMana()
                *BAR_RELSCALE_X, BAR_RELSCALE_Y));
            GetSprite("EXP").Scale(Simian::Vector2(static_cast<Simian::f32>(
                pAttribs->Experience())/pAttribs->MinExperienceGoal(), 1.0f));
            GetSprite("EXPGLOW").Scale(Simian::Vector2(static_cast<Simian::f32>(
                pAttribs->Experience())/pAttribs->MinExperienceGoal(), 1.0f));
            GetSprite("EXPBar").Scale(Simian::Vector2(static_cast<Simian::f32>(
                pAttribs->ExperienceGoal())/pAttribs->MinExperienceGoal()
                *BAR_RELSCALE_X, BAR_RELSCALE_Y));
            GetSprite("HPNail").Position(Simian::Vector2(
                GetSprite("HPBar").Position().X()+GetSprite("HPBar").BoundingBox().X()
                *GetSprite("HPBar").Scale().X()-winSize.Y()*0.01f,
                GetSprite("HPNail").Position().Y()));
            GetSprite("MPNail").Position(Simian::Vector2(
                GetSprite("MPBar").Position().X()+GetSprite("MPBar").BoundingBox().X()
                *GetSprite("MPBar").Scale().X()-winSize.Y()*0.01f,
                GetSprite("MPNail").Position().Y()));
            GetSprite("EXPNail").Position(Simian::Vector2(
                GetSprite("EXPBar").Position().X()+GetSprite("EXPBar").BoundingBox().X()
                *GetSprite("EXPBar").Scale().X()-winSize.Y()*0.01f,
                GetSprite("EXPNail").Position().Y()));
            
            //--SPARKY PARTICLES ON UI EFFECT
            Simian::Entity* eparticles = NULL;
            Simian::CUIResize* transform = NULL;
            Simian::CParticleSystem* particles = NULL;
            if (!checkBars_ && pAttribs->MaxHealth() != prevMaxHP)
            {
                //--For the right edge
                //eparticles = ParentEntity()->ParentScene()->CreateEntity(E_UI_SPARK_PARTICLE);
                //ParentScene()->AddEntity(eparticles);
                //eparticles->ComponentByType(Simian::C_PARTICLESYSTEM, particles);
                //eparticles->ComponentByType(Simian::C_UIRESIZE, transform);
                //Sprite* nail = GetSpritePtr("HPNail");
                //transform->Position(Simian::Vector3(
                //    nail->Position().X()/screenSize_.Y()+0.005f,
                //    nail->Position().Y()/screenSize_.Y(), 0));
                //particles->ParticleSystem()->Enabled(true);

                eparticles = ParentEntity()->ParentScene()->CreateEntity(E_UI_SPARK_PARTICLE);
                ParentScene()->AddEntity(eparticles);
                eparticles->ComponentByType(Simian::C_PARTICLESYSTEM, particles);
                eparticles->ComponentByType(Simian::C_UIRESIZE, transform);
                Sprite* nail = GetSpritePtr("HPBar");
                transform->Position(Simian::Vector3(
                    nail->Position().X()/screenSize_.Y()+0.015f,
                    nail->Position().Y()/screenSize_.Y()-0.025f, 0));
                particles->ParticleSystem()->Enabled(true);
            }
            if (!checkBars_ &&
                std::abs(pAttribs->MaxMana() - prevMaxMP) > Simian::Math::EPSILON)
            {
                //eparticles = ParentEntity()->ParentScene()->CreateEntity(E_UI_SPARK_PARTICLE);
                //ParentScene()->AddEntity(eparticles);
                //eparticles->ComponentByType(Simian::C_PARTICLESYSTEM, particles);
                //eparticles->ComponentByType(Simian::C_UIRESIZE, transform);
                //Sprite* nail = GetSpritePtr("MPNail");
                //transform->Position(Simian::Vector3(
                //    nail->Position().X()/screenSize_.Y()+0.005f,
                //    nail->Position().Y()/screenSize_.Y(), 0));
                //particles->ParticleSystem()->Enabled(true);

                eparticles = ParentEntity()->ParentScene()->CreateEntity(E_UI_SPARK_PARTICLE);
                ParentScene()->AddEntity(eparticles);
                eparticles->ComponentByType(Simian::C_PARTICLESYSTEM, particles);
                eparticles->ComponentByType(Simian::C_UIRESIZE, transform);
                Sprite* nail = GetSpritePtr("MPBar");
                transform->Position(Simian::Vector3(
                    nail->Position().X()/screenSize_.Y()+0.005f,
                    nail->Position().Y()/screenSize_.Y()-0.025f, 0));
                particles->ParticleSystem()->Enabled(true);
            }

            //update all value
            prevHP = pAttribs->Health();
            prevMP = pAttribs->Mana();
            prevMaxHP = pAttribs->MaxHealth();
            prevMaxMP = pAttribs->MaxMana();
            prevEXP = pAttribs->Experience();
        }
        if (hpFlash > 0.0f)
            hpFlash -= hpFlash - Simian::Game::Instance().FrameTime() == 0.0f ?
            hpFlash - 10 : Simian::Game::Instance().FrameTime();
        if (hpFlash < 0.0f)
        {
            GetSpritePtr("HP")->UV(&uiFrames[UI_HP]);
            hpFlash = 0.0f;
        }

        static Simian::f32 opacInc = 0.0f;
        if (pAttribs->Experience() == pAttribs->ExperienceGoal())
        {
            Sprite* pGlow = GetSpritePtr("EXPGLOW");
            pGlow->Opacity(pGlow->Opacity()+opacInc*dt_);
            if (pGlow->Opacity() >= 1.0f)
                opacInc = -1.0f;
            else if (pGlow->Opacity() <= 0.0f)
                opacInc = 1.0f;
            pGlow->Opacity(Simian::Math::Clamp<Simian::f32>
                          (pGlow->Opacity(), 0.0f, 1.0f));
        } else {
            GetSprite("EXPGLOW").Opacity(0.0f);
            opacInc = 0.0f;
        }

        checkBars_ = false;
    }
    void GCUI::UpdateMenu_(void)
    {
        const Simian::f32 opacLow = 0.7f;
        static Simian::f32 intp = 0.0f;
        Simian::Vector2 mouseVec = Simian::Mouse::GetMouseClientPosition();
        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        mouseVec.X(mouseVec.X()-winSize.X()/2.0f);
        mouseVec.Y(-(mouseVec.Y()-winSize.Y()/2.0f));

        if (Simian::Keyboard::IsTriggered(Simian::Keyboard::KEY_ESCAPE))
        {
            if (GCPlayerLogic::Instance()->PlayerControlOverride() ||
                GCUpgradeExchange::Exchanging())
                return;
            gameStatus_ = gameStatus_==GAME_RUNNING ? GAME_PAUSED :
                fsm_.CurrentState()==MENU_READY ? GAME_RUNNING : gameStatus_;
            if (gameStatus_ == GAME_PAUSED)
            {
                if (fsm_.CurrentState()==MENU_READY)
                {
                    LoadMenu_(winSize, opacLow);
                    Simian::Game::Instance().TimeMultiplier(0.0f);
                    if (jukebox_)
                        jukebox_->VolumeMultiplier(
                            Simian::CJukebox::VOLUME_GAME_PAUSE_MULTIPLIER);
                    intp = 0.0f;
                }
                if (entity_)
                {
                    ParentScene()->RemoveEntity(entity_);
                    entity_ = NULL;
                }
                fsm_.ChangeState(MENU_READY);
            }else{
                UnloadMenu_();
                Simian::Game::Instance().TimeMultiplier(1.0f);
                if (jukebox_)
                    jukebox_->VolumeMultiplier(
                        Simian::CJukebox::VOLUME_ORIGINAL_MULTIPLIER);
            }
        }

        //----------------------------------------------------------------------
        fsm_.Update(dt_);
        if (fsm_.CurrentState() != MENU_READY)
            return;

        //----------------------------------------------------------------------
        if (intp < 1.0f)
        {
            intp += Simian::Game::Instance().FrameTime();
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            Sprite* temp = GetSpritePtr("PauseMSG");
            if (!temp)
                return;
            MenuEnterAnimation_(temp, 100.0f, 0, intp, 2.0f, false, opacLow);
            temp = GetSpritePtr("WinBtn");
            MenuEnterAnimation_(temp,
                temp->BoundingBox().Y()*0.5f-temp->BoundingBox().Y()*0.5f,
                temp->BoundingBox().Y()*0.5f, intp, 8.0f);
            temp = GetSpritePtr("Option");
            MenuEnterAnimation_(temp,
                temp->BoundingBox().X()*0.5f,
                -temp->BoundingBox().X()*0.5f, intp, 4.0f, true, opacLow);
            temp = GetSpritePtr("HowToPlay");
            MenuEnterAnimation_(temp,
                temp->BoundingBox().X()*0.5f,
                -temp->BoundingBox().X()*0.5f, intp, 2.0f, true, opacLow);
            temp = GetSpritePtr("Credit");
            MenuEnterAnimation_(temp,
                temp->BoundingBox().X()*0.5f,
                -temp->BoundingBox().X()*0.5f, intp, 1.5f, true, opacLow);
            temp = GetSpritePtr("Restart");
            MenuEnterAnimation_(temp,
                temp->BoundingBox().X()*0.5f,
                -temp->BoundingBox().X()*0.5f, intp, 1.25f, true, opacLow);
            temp = GetSpritePtr("Quit");
            MenuEnterAnimation_(temp,
                temp->BoundingBox().X()*0.5f,
                -temp->BoundingBox().X()*0.5f, intp, 1.0f, true, opacLow);
            return;
        }

        //----------------------------------------------------------------------
        if ((Simian::Mouse::OnMouseMove() ||
            Simian::Mouse::IsPressed(Simian::Mouse::KEY_LBUTTON) ||
            Simian::Mouse::IsReleased(Simian::Mouse::KEY_LBUTTON))
            && gameStatus_ == GAME_PAUSED)
        {
            //----------------------------PAUSE---------------------------------
            Sprite* sptr = GetSpritePtr("PauseMSG");
            if (sptr)
            {
                sptr->OnHover(mouseVec) ? sptr->Opacity(1.0f) :
                    sptr->Opacity(opacLow);
                if (sptr->OnRelease(mouseVec))
                {
                    gameStatus_ = GAME_RUNNING;
                    UnloadMenu_();
                    Simian::Game::Instance().TimeMultiplier(1.0f);
                }
            }
            //---------------------------OPTION---------------------------------
            sptr = GetSpritePtr("Option");
            if (sptr)
            {
                sptr->OnHover(mouseVec) ? sptr->Opacity(1.0f) :
                    sptr->Opacity(opacLow);
                if (sptr->OnPress(mouseVec))
                    sptr->UV(&uiFrames[UI_OPTION_DOWN]);
                else
                    sptr->UV(&uiFrames[UI_OPTION]);
                if (sptr->OnRelease(mouseVec))
                {
                    fsm_.ChangeState(MENU_OPTION);
                    entity_ = ParentScene()->CreateEntity(E_OPTIONSUI);
                    Simian::CTransform* transform;
                    entity_->ComponentByType(Simian::C_TRANSFORM, transform);
                    transform->Position(transform->Position() + Simian::Vector3(0, 0, -5.0f));
                    ParentScene()->AddEntity(entity_);

                    GCOptionsMenu* optionsMenu;
                    entity_->ComponentByType(GC_OPTIONSMENU, optionsMenu);
                    optionsMenu->CloseCallback(Simian::Delegate::
                        CreateDelegate<GCUI, &GCUI::MenuCloseCallBack_>(this));
                }
            }
            //----------------------------HOWTO---------------------------------
            sptr = GetSpritePtr("HowToPlay");
            if (sptr)
            {
                sptr->OnHover(mouseVec) ? sptr->Opacity(1.0f) :
                    sptr->Opacity(opacLow);
                if (sptr->OnPress(mouseVec))
                    sptr->UV(&uiFrames[UI_HOWTOPLAY_DOWN]);
                else
                    sptr->UV(&uiFrames[UI_HOWTOPLAY]);
                if (sptr->OnRelease(mouseVec))
                {
                    fsm_.ChangeState(MENU_HOWTOPLAY);
                    entity_ = ParentScene()->CreateEntity(E_HOWTOPLAYUI);
                    Simian::CTransform* transform;
                    entity_->ComponentByType(Simian::C_TRANSFORM, transform);
                    transform->Position(transform->Position() + Simian::Vector3(0, 0, -5.0f));
                    ParentScene()->AddEntity(entity_);

                    GCHowToPlay* howToPlayMenu;
                    entity_->ComponentByType(GC_HOWTOPLAYMENU, howToPlayMenu);
                    howToPlayMenu->CloseCallback(Simian::Delegate::
                        CreateDelegate<GCUI, &GCUI::MenuCloseCallBack_>(this));
                }
            }
            //----------------------------RESTART-------------------------------
            sptr = GetSpritePtr("Restart");
            if (sptr)
            {
                sptr->OnHover(mouseVec) ? sptr->Opacity(1.0f) :
                    sptr->Opacity(opacLow);
            if (sptr->OnPress(mouseVec))
                sptr->UV(&uiFrames[UI_RESTART_DOWN]);
            else
                sptr->UV(&uiFrames[UI_RESTART]);
                if (sptr->OnRelease(mouseVec))
                {
                    fsm_.ChangeState(MENU_RESTART);
                }
            }
            //-----------------------------QUIT---------------------------------
            sptr = GetSpritePtr("Quit");
            if (sptr)
            {
                sptr->OnHover(mouseVec) ? sptr->Opacity(1.0f) :
                    sptr->Opacity(opacLow);
                if (sptr->OnPress(mouseVec))
                    sptr->UV(&uiFrames[UI_QUIT_DOWN]);
                else
                    sptr->UV(&uiFrames[UI_QUIT]);
                if (sptr->OnRelease(mouseVec))
                {
                    fsm_.ChangeState(MENU_QUIT);
                }
            }
            //-----------------------------CREDIT-------------------------------
            sptr = GetSpritePtr("Credit");
            if (sptr)
            {
                sptr->OnHover(mouseVec) ? sptr->Opacity(1.0f) :
                    sptr->Opacity(opacLow);
                if (sptr->OnPress(mouseVec))
                    sptr->UV(&uiFrames[UI_CREDIT_DOWN]);
                else
                    sptr->UV(&uiFrames[UI_CREDIT]);
                if (sptr->OnRelease(mouseVec))
                {
                    fsm_.ChangeState(MENU_CREDIT);
                    entity_ = ParentScene()->CreateEntity(E_CREDITSUI);
                    Simian::CTransform* transform;
                    entity_->ComponentByType(Simian::C_TRANSFORM, transform);
                    transform->Position(transform->Position() + Simian::Vector3(0, 0, -5.0f));
                    ParentScene()->AddEntity(entity_);

                    GCCredits* creditsMenu;
                    entity_->ComponentByType(GC_CREDITSMENU, creditsMenu);
                    creditsMenu->CloseCallback(Simian::Delegate::
                        CreateDelegate<GCUI, &GCUI::MenuCloseCallBack_>(this));
                }
            }
        }
    }
    void GCUI::UpdateBoss_(void)
    {
        const Simian::f32 flashTime = 0.1f;
        static Simian::f32 hpFlash = flashTime;

        if (bossID_ == 0 || bossPtr_ == NULL)
            return;

        GCAttributes* pAttribs = bossPtr_->Attributes();
        static Simian::s32 prevHP = pAttribs->Health();
        if (pAttribs->Health()!=prevHP)
        {
            Sprite* pHP = GetSpritePtr("BossHP");
            //flash effect
            if (pAttribs->Health()!=prevHP)
            {
                pHP->UV(&uiFrames[UI_FLASH]);
                hpFlash = flashTime;
            }
            pHP->Scale(Simian::Vector2(static_cast<Simian::f32>(
                pAttribs->Health())/pAttribs->MaxHealth(), 1.0f));
            //update all value
            prevHP = pAttribs->Health();
        }
        if (hpFlash > 0.0f)
            hpFlash -= hpFlash - Simian::Game::Instance().FrameTime() == 0.0f ?
                hpFlash - 10 : Simian::Game::Instance().FrameTime();
        if (hpFlash < 0.0f)
        {
            GetSpritePtr("BossHP")->UV(&uiFrames[UI_HP]);
            hpFlash = 0.0f;
        }
    }
    void GCUI::ResizeAllSprites_(void)
    {
        //--check win size
        const Simian::f32 opac = 0.7f;
        Simian::Vector2 winSize = Simian::Game::Instance().Size();
        if (std::abs(winSize.X() - screenSize_.X()) < Simian::Math::EPSILON &&
            std::abs(winSize.Y() - screenSize_.Y()) < Simian::Math::EPSILON)
            return;

        UnloadMenu_();
        LoadMenu_(winSize, opac);
        GetSprite("PauseMSG").Pivot(Simian::Vector2(GetSprite("PauseMSG").Pivot().X(), 0));
        if (!GCPlayerLogic::Instance()->PlayerControlOverride())
        {
            UnloadUI_();
            LoadUI_();
        }

        //--update win size
        screenSize_ = winSize;
        //--Boss checks
        if (bossID_ != 0)
        {
            Simian::u32 tempID = bossID_;
            GCAIBase* tempBoss = bossPtr_;
            EndBossBattle();
            StartBossBattle(tempBoss, tempID);
        }
        //--Skill icons
        //if (GCGestureInterpreter::Instance()->GetSkills().Skills() > 0 &&
        //    !GCPlayerLogic::Instance()->PlayerControlOverride())
        //{
            UnloadSkills(true);
            LoadSkills();
        //}
    }
    void GCUI::MenuRestartEnter_(Simian::DelegateParameter&)
    {
        const Simian::f32 opacLow = 0.0f;
        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        CreateSprite("WinRestart", -winSize.Y()*0.166f, winSize.Y()*0.05f,
            winSize.Y()*0.56f, winSize.Y()*0.11f, 1.0f, 1.0f, 0.0f, 0.0f,
            -winSize.Y()*0.56f*0.5f, winSize.Y()*0.11f*0.5f);
        GetSprite("WinRestart").UV(&uiFrames[UI_WIN_RESTART]);
        CreateSprite("Yes", -winSize.Y()*0.153f, -winSize.Y()*0.075f,
            winSize.Y()*0.24f, winSize.Y()*0.06f, 1.0f, 1.0f, 0, opacLow,
            -winSize.Y()*0.24f*0.5f, winSize.Y()*0.06f*0.5f);
        GetSprite("Yes").UV(&uiFrames[UI_YES]);
        CreateSprite("No", winSize.Y()*0.138f, -winSize.Y()*0.075f,
            winSize.Y()*0.24f, winSize.Y()*0.06f, 1.0f, 1.0f, 0, opacLow,
            -winSize.Y()*0.24f*0.5f, winSize.Y()*0.06f*0.5f);
        GetSprite("No").UV(&uiFrames[UI_NO]);
    }
    void GCUI::MenuRestartUpdate_(Simian::DelegateParameter&)
    {
        const Simian::f32 opacLow = 0.7f;
        Simian::Vector2 mouseVec = Simian::Mouse::GetMouseClientPosition();
        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        mouseVec.X(mouseVec.X()-winSize.X()/2.0f);
        mouseVec.Y(-(mouseVec.Y()-winSize.Y()/2.0f));

        Sprite* tempIn = GetSpritePtr("WinRestart");
        if (tempIn && tempIn->Opacity() < 1.0f)
        {
            tempIn->Opacity(Simian::Math::Clamp(
                tempIn->Opacity() + Simian::Game::Instance().FrameTime()*2.0f,
                0.0f, 1.0f));
            Simian::f32 val = Simian::Interpolation::Interpolate
                <Simian::f32>(-tempIn->BoundingBox().Y()*0.5f,
                0, Simian::Math::Clamp(tempIn->Opacity(), 0.0f, 1.0f), Simian::Interpolation::EaseCubicOut);
            tempIn->Pivot(Simian::Vector2(tempIn->Pivot().X(), val));

            Sprite* temp = GetSpritePtr("Yes");
            if (temp)
            {
                Simian::f32 val = Simian::Interpolation::Interpolate
                    <Simian::f32>(temp->BoundingBox().Y(),
                    0, Simian::Math::Clamp(tempIn->Opacity(), 0.0f, 1.0f), Simian::Interpolation::EaseCubicOut);
                temp->Pivot(Simian::Vector2(temp->Pivot().X(), val));
                temp->Opacity(Simian::Math::Clamp(tempIn->Opacity()*2, 0.0f, 1.0f)*opacLow);
            }

            temp = GetSpritePtr("No");
            if (temp)
            {
                Simian::f32 val = Simian::Interpolation::Interpolate
                    <Simian::f32>(temp->BoundingBox().Y(),
                    0, Simian::Math::Clamp(tempIn->Opacity(), 0.0f, 1.0f), Simian::Interpolation::EaseCubicOut);
                temp->Pivot(Simian::Vector2(temp->Pivot().X(), val));
                temp->Opacity(Simian::Math::Clamp(tempIn->Opacity(), 0.0f, 1.0f)*opacLow);
            }
            return; //--no clicking till done
        }

        //------------------------------YES---------------------------------
        Sprite* sptr = GetSpritePtr("Yes");
        if (sptr)
        {
            sptr->OnHover(mouseVec) ? sptr->Opacity(1.0f) :
                sptr->Opacity(opacLow);
        if (sptr->OnPress(mouseVec))
            sptr->UV(&uiFrames[UI_YES_DOWN]);
        else
            sptr->UV(&uiFrames[UI_YES]);
        if (sptr->OnRelease(mouseVec))
        {
            fsm_.ChangeState(MENU_READY);
            SaveManager::Instance().LoadCheckpoint();
            //Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            //StateLoading::Instance().NextState(StateGame::InstancePtr());
        }
        }
        //-------------------------------NO---------------------------------
        sptr = GetSpritePtr("No");
        if (sptr)
        {
            sptr->OnHover(mouseVec) ? sptr->Opacity(1.0f) :
                sptr->Opacity(opacLow);
        if (sptr->OnPress(mouseVec))
            sptr->UV(&uiFrames[UI_NO_DOWN]);
        else
            sptr->UV(&uiFrames[UI_NO]);
        if (sptr->OnRelease(mouseVec))
        {
            fsm_.ChangeState(MENU_READY);
        }
        }
    }
    void GCUI::MenuRestartExit_(Simian::DelegateParameter&)
    {
        DeleteSprite(GetSprite("WinRestart").SpriteID());
        DeleteSprite(GetSprite("Yes").SpriteID());
        DeleteSprite(GetSprite("No").SpriteID());
    }
    void GCUI::MenuQuitEnter_(Simian::DelegateParameter&)
    {
        const Simian::f32 opacLow = 0.0f;
        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        CreateSprite("WinQuit", -winSize.Y()*0.166f, winSize.Y()*0.05f,
            winSize.Y()*0.56f, winSize.Y()*0.11f, 1.0f, 1.0f, 0.0f, 0.0f,
            -winSize.Y()*0.56f*0.5f, winSize.Y()*0.11f*0.5f);
        GetSprite("WinQuit").UV(&uiFrames[UI_WIN_QUIT]);
        CreateSprite("Yes", -winSize.Y()*0.153f, -winSize.Y()*0.075f,
            winSize.Y()*0.24f, winSize.Y()*0.06f, 1.0f, 1.0f, 0, opacLow,
            -winSize.Y()*0.24f*0.5f, winSize.Y()*0.06f*0.5f);
        GetSprite("Yes").UV(&uiFrames[UI_YES]);
        CreateSprite("No", winSize.Y()*0.138f, -winSize.Y()*0.075f,
            winSize.Y()*0.24f, winSize.Y()*0.06f, 1.0f, 1.0f, 0, opacLow,
            -winSize.Y()*0.24f*0.5f, winSize.Y()*0.06f*0.5f);
        GetSprite("No").UV(&uiFrames[UI_NO]);
    }
    void GCUI::MenuQuitUpdate_(Simian::DelegateParameter&)
    {
        const Simian::f32 opacLow = 0.7f;
        Simian::Vector2 mouseVec = Simian::Mouse::GetMouseClientPosition();
        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        mouseVec.X(mouseVec.X()-winSize.X()/2.0f);
        mouseVec.Y(-(mouseVec.Y()-winSize.Y()/2.0f));

        Sprite* tempIn = GetSpritePtr("WinQuit");
        if (tempIn && tempIn->Opacity() < 1.0f)
        {
            tempIn->Opacity(Simian::Math::Clamp(
                tempIn->Opacity() + Simian::Game::Instance().FrameTime()*2.0f,
                0.0f, 1.0f));
            Simian::f32 val = Simian::Interpolation::Interpolate
                <Simian::f32>(-tempIn->BoundingBox().Y()*0.5f,
                0, Simian::Math::Clamp(tempIn->Opacity(), 0.0f, 1.0f), Simian::Interpolation::EaseCubicOut);
            tempIn->Pivot(Simian::Vector2(tempIn->Pivot().X(), val));

            Sprite* temp = GetSpritePtr("Yes");
            if (temp)
            {
                Simian::f32 val = Simian::Interpolation::Interpolate
                    <Simian::f32>(temp->BoundingBox().Y(),
                    0, Simian::Math::Clamp(tempIn->Opacity(), 0.0f, 1.0f), Simian::Interpolation::EaseCubicOut);
                temp->Pivot(Simian::Vector2(temp->Pivot().X(), val));
                temp->Opacity(Simian::Math::Clamp(tempIn->Opacity()*2, 0.0f, 1.0f)*opacLow);
            }

            temp = GetSpritePtr("No");
            if (temp)
            {
                Simian::f32 val = Simian::Interpolation::Interpolate
                    <Simian::f32>(temp->BoundingBox().Y(),
                    0, Simian::Math::Clamp(tempIn->Opacity(), 0.0f, 1.0f), Simian::Interpolation::EaseCubicOut);
                temp->Pivot(Simian::Vector2(temp->Pivot().X(), val));
                temp->Opacity(Simian::Math::Clamp(tempIn->Opacity(), 0.0f, 1.0f)*opacLow);
            }
            return; //--no clicking till done
        }

        //------------------------------YES---------------------------------
        Sprite* sptr = GetSpritePtr("Yes");
        if (sptr)
        {
            sptr->OnHover(mouseVec) ? sptr->Opacity(1.0f) :
                sptr->Opacity(opacLow);
            if (sptr->OnPress(mouseVec))
                sptr->UV(&uiFrames[UI_YES_DOWN]);
            else
                sptr->UV(&uiFrames[UI_YES]);
            if (sptr->OnRelease(mouseVec))
            {
                fsm_.ChangeState(MENU_READY);
                Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
                StateLoading::Instance().NextState(StateGame::InstancePtr());
                StateGame::Instance().LevelFile("Data/Scenes/MainMenu.scn");
            }
        }
        //-------------------------------NO---------------------------------
        sptr = GetSpritePtr("No");
        if (sptr)
        {
            sptr->OnHover(mouseVec) ? sptr->Opacity(1.0f) :
                sptr->Opacity(opacLow);
            if (sptr->OnPress(mouseVec))
                sptr->UV(&uiFrames[UI_NO_DOWN]);
            else
                sptr->UV(&uiFrames[UI_NO]);
            if (sptr->OnRelease(mouseVec))
            {
                fsm_.ChangeState(MENU_READY);
            }
        }
    }
    void GCUI::MenuQuitExit_(Simian::DelegateParameter&)
    {
        DeleteSprite(GetSprite("WinQuit").SpriteID());
        DeleteSprite(GetSprite("Yes").SpriteID());
        DeleteSprite(GetSprite("No").SpriteID());
    }
    void GCUI::MenuCloseCallBack_(Simian::DelegateParameter&)
    {
        entity_ = NULL;
        fsm_.ChangeState(MENU_READY);
    }
    void GCUI::UpdateVB_(void)
    {
        for (Simian::u32 i=0; i<spriteCount_; ++i)
        {
            Simian::u32 j = i*4;
            Hexa32 newAlp(vertices_[j].Diffuse);
            for (Simian::u32 k=0; k<4; ++k)
                newAlp.Byte[k] = static_cast<Simian::u8>(255*sprites_[i].Opacity());
            vertices_[j].Diffuse = vertices_[j+1].Diffuse =
                vertices_[j+2].Diffuse = vertices_[j+3].Diffuse = newAlp.Full;

            vertices_[j].X = sprites_[i].Position().X() - 
                (sprites_[i].BoundingBox().X() + sprites_[i].Pivot().X()*2)
                *0.5f*sprites_[i].Scale().X(),
                vertices_[j].Y = sprites_[i].Position().Y() - 
                (sprites_[i].BoundingBox().Y() + sprites_[i].Pivot().Y()*2)
                *0.5f*sprites_[i].Scale().Y(),
                vertices_[j].U = sprites_[i].UV()[0].X(); vertices_[j].V = sprites_[i].UV()[0].Y();

            vertices_[j+1].X = sprites_[i].Position().X() + 
                (sprites_[i].BoundingBox().X() - sprites_[i].Pivot().X()*2)
                *0.5f*sprites_[i].Scale().X(),
                vertices_[j+1].Y = sprites_[i].Position().Y() - 
                (sprites_[i].BoundingBox().Y() + sprites_[i].Pivot().Y()*2)
                *0.5f*sprites_[i].Scale().Y(),
                vertices_[j+1].U = sprites_[i].UV()[1].X(); vertices_[j+1].V = sprites_[i].UV()[1].Y();

            vertices_[j+2].X = sprites_[i].Position().X() + 
                (sprites_[i].BoundingBox().X() - sprites_[i].Pivot().X()*2)
                *0.5f*sprites_[i].Scale().X(),
                vertices_[j+2].Y = sprites_[i].Position().Y() + 
                (sprites_[i].BoundingBox().Y() - sprites_[i].Pivot().Y()*2)
                *0.5f*sprites_[i].Scale().Y(),
                vertices_[j+2].U = sprites_[i].UV()[2].X(); vertices_[j+2].V = sprites_[i].UV()[2].Y();

            vertices_[j+3].X = sprites_[i].Position().X() - 
                (sprites_[i].BoundingBox().X() + sprites_[i].Pivot().X()*2)
                *0.5f*sprites_[i].Scale().X(),
                vertices_[j+3].Y = sprites_[i].Position().Y() + 
                (sprites_[i].BoundingBox().Y() - sprites_[i].Pivot().Y()*2)
                *0.5f*sprites_[i].Scale().Y(),
                vertices_[j+3].U = sprites_[i].UV()[3].X(); vertices_[j+3].V = sprites_[i].UV()[3].Y();
        }
    }
    void GCUI::GenerateUIFrames_(void)
    {
        uiFrames.clear();
        GCSpriteEditor::GetUVFromFile("UI/UV_UI.xml", uiFrames, UI_TOTAL_FRAMES);
    }
    void GCUI::LoadMenu_(Simian::Vector2& winSize, Simian::f32 opacLow)
    {
        Simian::Entity* entity = ParentScene()->EntityByType(E_SPIRITOFDEMAE);
        if (entity)
        {
            GCMousePlaneIntersection* spirit;
            entity->ComponentByType(GC_MOUSEPLANEINTERSECTION, spirit);
            spirit->Active(false);
        }
        CreateSprite("WinBtn", -winSize.X()*0.5f, winSize.Y()*0.25f,
            winSize.Y()*0.37f, winSize.Y()*0.47f, 1.0f, 1.0f, 0, 1.0f,
            -winSize.Y()*0.37f*0.5f, winSize.Y()*0.5f*0.47f);
        GetSprite("WinBtn").UV(&uiFrames[UI_WIN_BTN]);
        CreateSprite("PauseMSG", winSize.X()*0.5f-winSize.Y()*0.275f, -winSize.Y()*0.436f,
            winSize.Y()*0.24f, winSize.Y()*0.07f, 1.0f, 1.0f, 0, opacLow,
            -winSize.Y()*0.24f*0.5f, winSize.Y()*0.07f*0.5f);
        GetSprite("PauseMSG").UV(&uiFrames[UI_PAUSE]);
        CreateSprite("Option", -winSize.X()*0.5f+winSize.Y()*0.055f, winSize.Y()*0.2f,
            winSize.Y()*0.25f, winSize.Y()*0.056f, 1.0f, 1.0f, 0, opacLow,
            -winSize.Y()*0.25f*0.5f, winSize.Y()*0.056f);
        GetSprite("Option").UV(&uiFrames[UI_OPTION]);
        CreateSprite("HowToPlay", -winSize.X()*0.5f+winSize.Y()*0.055f, winSize.Y()*0.130f,
            winSize.Y()*0.25f, winSize.Y()*0.056f, 1.0f, 1.0f, 0, opacLow,
            -winSize.Y()*0.25f*0.5f, winSize.Y()*0.056f);
        GetSprite("HowToPlay").UV(&uiFrames[UI_HOWTOPLAY]);
        CreateSprite("Credit", -winSize.X()*0.5f+winSize.Y()*0.055f, winSize.Y()*0.06f,
            winSize.Y()*0.25f, winSize.Y()*0.056f, 1.0f, 1.0f, 0, opacLow,
            -winSize.Y()*0.25f*0.5f, winSize.Y()*0.056f);
        GetSprite("Credit").UV(&uiFrames[UI_CREDIT]);
        CreateSprite("Restart", -winSize.X()*0.5f+winSize.Y()*0.055f, -winSize.Y()*0.01f,
            winSize.Y()*0.25f, winSize.Y()*0.056f, 1.0f, 1.0f, 0, opacLow,
            -winSize.Y()*0.25f*0.5f, winSize.Y()*0.056f);
        GetSprite("Restart").UV(&uiFrames[UI_RESTART]);
        CreateSprite("Quit", -winSize.X()*0.5f+winSize.Y()*0.055f, -winSize.Y()*0.08f,
            winSize.Y()*0.25f, winSize.Y()*0.056f, 1.0f, 1.0f, 0, opacLow,
            -winSize.Y()*0.25f*0.5f, winSize.Y()*0.056f);
        GetSprite("Quit").UV(&uiFrames[UI_QUIT]);
    }
    void GCUI::UnloadMenu_(void)
    {
        Simian::Entity* entity = ParentScene()->EntityByType(E_SPIRITOFDEMAE);
        if (entity)
        {
            GCMousePlaneIntersection* spirit;
            entity->ComponentByType(GC_MOUSEPLANEINTERSECTION, spirit);
            spirit->Active(true);
        }
        DeleteSprite(GetSprite("WinBtn").SpriteID());
        DeleteSprite(GetSprite("PauseMSG").SpriteID());
        DeleteSprite(GetSprite("Option").SpriteID());
        DeleteSprite(GetSprite("HowToPlay").SpriteID());
        DeleteSprite(GetSprite("Credit").SpriteID());
        DeleteSprite(GetSprite("Restart").SpriteID());
        DeleteSprite(GetSprite("Quit").SpriteID());
    }
    void GCUI::LoadUI_(void)
    {
        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        Simian::f32 opac = 0.5f, nailPadding = winSize.Y()*0.01f;

        CreateSprite("HP",-winSize.X()*0.5f+winSize.Y()*0.1015f, winSize.Y()*0.48f, //hp
            winSize.Y()*0.293f, winSize.Y()*0.028f,
            1.0f, 1.0f, 0.0f, 1.0f, -winSize.Y()*0.293f*0.5f, winSize.Y()*0.028f);
        GetSprite("HP").UV(&uiFrames[UI_HP]);
        CreateSprite("MP",-winSize.X()*0.5f+winSize.Y()*0.1015f, winSize.Y()*0.445f, //mp
            winSize.Y()*0.293f, winSize.Y()*0.028f,
            1.0f, 1.0f, 0.0f, 1.0f, -winSize.Y()*0.293f*0.5f, winSize.Y()*0.028f);
        GetSprite("MP").UV(&uiFrames[UI_MP]);
        CreateSprite("EXP",-winSize.X()*0.5f+winSize.Y()*0.1015f, winSize.Y()*0.41f, //exp
            winSize.Y()*0.293f, winSize.Y()*0.028f,
            1.0f, 1.0f, 0.0f, 1.0f, -winSize.Y()*0.293f*0.5f, winSize.Y()*0.028f);
        GetSprite("EXP").UV(&uiFrames[UI_EXP]);
        CreateSprite("EXPGLOW",-winSize.X()*0.5f+winSize.Y()*0.1015f, winSize.Y()*0.41f, //exp
            winSize.Y()*0.293f, winSize.Y()*0.028f,
            1.0f, 1.0f, 0.0f, 1.0f, -winSize.Y()*0.293f*0.5f, winSize.Y()*0.028f);
        GetSprite("EXPGLOW").UV(&uiFrames[UI_EXPGLOW]);

        CreateSprite("HPBar",-winSize.X()*0.5f+winSize.Y()*0.1015f, winSize.Y()*0.473f, //hp
            winSize.Y()*0.293f, winSize.Y()*0.028f, BAR_RELSCALE_X,
            BAR_RELSCALE_Y*GetSprite("HP").Scale().X(), 0.0f, 1.0f, -winSize.Y()*0.293f*0.5f, winSize.Y()*0.028f*0.5f);
        GetSprite("HPBar").UV(&uiFrames[UI_BAR]);
        GetSprite("HPBar").Opacity(opac);
        CreateSprite("MPBar",-winSize.X()*0.5f+winSize.Y()*0.1015f, winSize.Y()*0.438f, //mp
            winSize.Y()*0.293f, winSize.Y()*0.028f, BAR_RELSCALE_X,
            BAR_RELSCALE_Y*GetSprite("MP").Scale().X(), 0.0f, 1.0f, -winSize.Y()*0.293f*0.5f, winSize.Y()*0.028f*0.5f);
        GetSprite("MPBar").UV(&uiFrames[UI_BAR]);
        GetSprite("MPBar").Opacity(opac);
        CreateSprite("EXPBar",-winSize.X()*0.5f+winSize.Y()*0.1015f, winSize.Y()*0.403f, //exp
            winSize.Y()*0.293f, winSize.Y()*0.028f, BAR_RELSCALE_X,
            BAR_RELSCALE_Y*GetSprite("EXP").Scale().X(), 0.0f, 1.0f, -winSize.Y()*0.293f*0.5f, winSize.Y()*0.028f*0.5f);
        GetSprite("EXPBar").UV(&uiFrames[UI_BAR]);
        GetSprite("EXPBar").Opacity(opac);

        CreateSprite("HPNail",GetSprite("HPBar").Position().X()+GetSprite("HPBar").BoundingBox().X()
            *GetSprite("HPBar").Scale().X()-nailPadding,
            GetSprite("HPBar").Position().Y()-GetSprite("HPBar").BoundingBox().Y()*GetSprite("HPBar").Scale().Y()/2,
            winSize.Y()*0.02f, winSize.Y()*0.02f,
            1.0f, 1.0f, 0.0f, 1.0f, winSize.Y()*0.02f*0.5f);
        GetSprite("HPNail").UV(&uiFrames[UI_NAIL]);
        CreateSprite("MPNail",GetSprite("MPBar").Position().X()+GetSprite("MPBar").BoundingBox().X()
            *GetSprite("MPBar").Scale().X()-nailPadding,
            GetSprite("MPBar").Position().Y()-GetSprite("MPBar").BoundingBox().Y()*GetSprite("MPBar").Scale().Y()/2,
            winSize.Y()*0.02f, winSize.Y()*0.02f,
            1.0f, 1.0f, 0.0f, 1.0f, winSize.Y()*0.02f*0.5f);
        GetSprite("MPNail").UV(&uiFrames[UI_NAIL]);
        CreateSprite("EXPNail",GetSprite("EXPBar").Position().X()+GetSprite("EXPBar").BoundingBox().X()
            *GetSprite("EXPBar").Scale().X()-nailPadding,
            GetSprite("EXPBar").Position().Y()-GetSprite("EXPBar").BoundingBox().Y()*GetSprite("EXPBar").Scale().Y()/2,
            winSize.Y()*0.02f, winSize.Y()*0.02f,
            1.0f, 1.0f, 0.0f, 1.0f, winSize.Y()*0.02f*0.5f);
        GetSprite("EXPNail").UV(&uiFrames[UI_NAIL]);
        
        CreateSprite("BG",-winSize.X()*0.5f, winSize.Y()*0.5f, //BG
            winSize.Y()*0.25f, winSize.Y()*0.17f,
            1.0f, 1.0f, 0.0f, 1.0f, -winSize.Y()*0.25f*0.5f, winSize.Y()*0.17f*0.5f);
        GetSprite("BG").UV(&uiFrames[UI_TLBG]);

        GCAttributes* pAttribs = GCPlayerLogic::Instance()->Attributes();
        GetSprite("HP").Scale(Simian::Vector2(static_cast<Simian::f32>(
            pAttribs->Health())/pAttribs->MinMaxHealth(), 1.0f));
        GetSprite("MP").Scale(Simian::Vector2(static_cast<Simian::f32>(
            pAttribs->Mana())/pAttribs->MinMaxMana(), 1.0f));
        GetSprite("HPBar").Scale(Simian::Vector2(static_cast<Simian::f32>(
            pAttribs->MaxHealth())/pAttribs->MinMaxHealth()
            *BAR_RELSCALE_X, BAR_RELSCALE_Y));
        GetSprite("MPBar").Scale(Simian::Vector2(static_cast<Simian::f32>(
            pAttribs->MaxMana())/pAttribs->MinMaxMana()
            *BAR_RELSCALE_X, BAR_RELSCALE_Y));
        GetSprite("EXP").Scale(Simian::Vector2(static_cast<Simian::f32>(
            pAttribs->Experience())/pAttribs->MinExperienceGoal(), 1.0f));
        GetSprite("EXPGLOW").Scale(Simian::Vector2(static_cast<Simian::f32>(
            pAttribs->Experience())/pAttribs->MinExperienceGoal(), 1.0f));
        GetSprite("EXPBar").Scale(Simian::Vector2(static_cast<Simian::f32>(
            pAttribs->ExperienceGoal())/pAttribs->MinExperienceGoal()
            *BAR_RELSCALE_X, BAR_RELSCALE_Y));
        GetSprite("HPNail").Position(Simian::Vector2(
            GetSprite("HPBar").Position().X()+GetSprite("HPBar").BoundingBox().X()
            *GetSprite("HPBar").Scale().X()-winSize.Y()*0.01f,
            GetSprite("HPNail").Position().Y()));
        GetSprite("MPNail").Position(Simian::Vector2(
            GetSprite("MPBar").Position().X()+GetSprite("MPBar").BoundingBox().X()
            *GetSprite("MPBar").Scale().X()-winSize.Y()*0.01f,
            GetSprite("MPNail").Position().Y()));
        GetSprite("EXPNail").Position(Simian::Vector2(
            GetSprite("EXPBar").Position().X()+GetSprite("EXPBar").BoundingBox().X()
            *GetSprite("EXPBar").Scale().X()-winSize.Y()*0.01f,
            GetSprite("EXPNail").Position().Y()));

        //--BossName
        if (GetSpritePtr("BossName") == NULL)
        {
            CreateSprite("BossName", 0.0f, winSize.Y()*-0.3f,
                winSize.Y()*0.41f, winSize.Y()*0.08f,
                1.0f, 1.0f, 0.0f, 0.0f);
            GetSprite("BossName").UV(&uiFrames[UI_CRIMSON_REVENANT]);
        }
        //--UI_EXP_MSG
        CreateSprite("EXPMessage0",GetSprite("EXPBar").Position().X()+GetSprite("EXPBar").BoundingBox().X()*0.49f,
            GetSprite("EXPBar").Position().Y()-GetSprite("EXPBar").BoundingBox().Y()*3.0f,
            winSize.Y()*0.32f, winSize.Y()*0.032f,
            1.0f, 1.0f, 0.0f, 0.0f);
        GetSprite("EXPMessage0").UV(&uiFrames[UI_EXP_MSG0]);
        CreateSprite("EXPMessage1",GetSprite("EXPBar").Position().X()+GetSprite("EXPBar").BoundingBox().X()*0.49f,
            GetSprite("EXPBar").Position().Y()-GetSprite("EXPBar").BoundingBox().Y()*3.0f,
            winSize.Y()*0.32f, winSize.Y()*0.032f,
            1.0f, 1.0f, 0.0f, 0.0f);
        GetSprite("EXPMessage1").UV(&uiFrames[UI_EXP_MSG1]);
    }
    void GCUI::UnloadUI_(void)
    {
        DeleteSprite(GetSprite("HP").SpriteID());
        DeleteSprite(GetSprite("MP").SpriteID());
        DeleteSprite(GetSprite("EXP").SpriteID());
        DeleteSprite(GetSprite("EXPGLOW").SpriteID());
        DeleteSprite(GetSprite("HPBar").SpriteID());
        DeleteSprite(GetSprite("MPBar").SpriteID());
        DeleteSprite(GetSprite("EXPBar").SpriteID());
        DeleteSprite(GetSprite("HPNail").SpriteID());
        DeleteSprite(GetSprite("MPNail").SpriteID());
        DeleteSprite(GetSprite("EXPNail").SpriteID());
        DeleteSprite(GetSprite("BG").SpriteID());
        if (bossName_.State == BossNameStruct::BOSSNAME_IDLE)
            DeleteSprite(GetSprite("BossName").SpriteID());
        DeleteSprite(GetSprite("EXPMessage0").SpriteID());
        DeleteSprite(GetSprite("EXPMessage1").SpriteID());
    }

    void GCUI::MenuEnterAnimation_(Sprite* temp,
        Simian::f32 startX,
        Simian::f32 startY,
        Simian::f32 intp,
        Simian::f32 intpMultiplier,
        bool horizontal,
        Simian::f32 opacMultiplier)
    {
        Simian::f32 val = Simian::Interpolation::Interpolate
            <Simian::f32>(startX, startY, //--Cascading effect
            Simian::Math::Clamp(intp*intpMultiplier, 0.0f, 1.0f),
            Simian::Interpolation::EaseCubicOut);
        if (horizontal)
            temp->Pivot(Simian::Vector2(val, temp->Pivot().Y()));
        else
            temp->Pivot(Simian::Vector2(temp->Pivot().X(), val));
        temp->Opacity(Simian::Math::Clamp(intp*intpMultiplier,
            0.0f, 1.0f)*opacMultiplier);
    }

    void GCUI::LoadSkills(bool animate, Simian::u32 id)
    {
        const Simian::u32 skillMax = 31;

        skillShow_ = true;

        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        Simian::u32 bitFlag = skillMax, i = 0;
        //GCGestureInterpreter::Instance()->GetSkills().Skills()
        std::stringstream sstr;
        if (!uiLoaded_)
        {
            while (bitFlag)
            {
                sstr.str("");
                sstr << "Icon" << i;
                bitFlag >>= 1;

                //--HACKPOS
                Simian::f32 xpos = 0.0f, ypos = 0.0f;
                switch (i)
                {
                case 0: 
                    xpos = -winSize.X()*0.5f+winSize.Y()*0.0448f;
                    ypos = winSize.Y()*0.42f;
                    break;
                case 1:
                    xpos = -winSize.X()*0.5f+winSize.Y()*0.0765f;
                    ypos = winSize.Y()*0.437f;
                    break;
                case 2:
                    xpos = -winSize.X()*0.5f+winSize.Y()*0.0765f;
                    ypos = winSize.Y()*0.400f;
                    break;
                }
                
                if ( ((bitFlag) & 1) && i<3) {
                    //if (animate && (i+Gesture::GCMD_SLASH==id) ) {
                    CreateSprite(sstr.str(),xpos/*-winSize.X()*0.5f+winSize.Y()*0.175f+winSize.Y()*0.08f*i*/,
                        ypos/*winSize.Y()*0.32f*/,
                        winSize.Y()*0.032f, winSize.Y()*0.032f,
                        (animate && i+Gesture::GCMD_SLASH==id) ? ICON_START_SCALE : 1.0f,
                        (animate && i+Gesture::GCMD_SLASH==id) ? ICON_START_SCALE : 1.0f,
                        0.0f, (animate && i+Gesture::GCMD_SLASH==id) ? 0.0f : 1.0f,
                        0, 0);
                    /*}else{
                        CreateSprite(sstr.str(),-winSize.X()*0.5f+winSize.Y()*0.14f+winSize.Y()*0.075f*i,
                            winSize.Y()*0.32f,
                            winSize.Y()*0.065f, winSize.Y()*0.065f, 1.0f, 1.0f,
                            0.0f, 1.0f, 0, 0);
                    }*/
                    GetSprite(sstr.str()).UV(&uiFrames[UI_ICON_SLASH+i]);
                }
                ++i;
            }
            uiLoaded_ = true;
        } else {
            bitFlag = GCGestureInterpreter::Instance()->GetSkills().Skills();
            while (bitFlag && i<3)
            {
                sstr.str("");
                sstr << "Icon" << i;
                bitFlag >>= 1;
                if ( (bitFlag) & 1) {
                    if (animate && (i+Gesture::GCMD_SLASH==id) ) {
                        Sprite& temp = GetSprite(sstr.str());
                        temp.Opacity(0.0f);
                        temp.Pivot(Simian::Vector2());
                    } else {
                        Sprite& temp = GetSprite(sstr.str());
                        temp.Pivot(Simian::Vector2());
                        temp.Opacity(1.0f);
                        temp.Scale(Simian::Vector2(1,1));
                    }
                    GetSprite(sstr.str()).UV(&uiFrames[UI_ICON_SLASH+i]);
                }
                ++i;
            }
        }
    }
    void GCUI::UpdateSkills(void)
    {
        Simian::u32 bitFlag =  GCGestureInterpreter::Instance()->GetSkills().Skills()
            , i = 0;
        std::stringstream sstr;
        while (i<3)
        {
            sstr.str("");
            sstr << "Icon" << i;
            bitFlag >>= 1;
            Sprite* temp = GetSpritePtr( sstr.str() );
            if ((bitFlag & 1) && temp && temp->Opacity() < 1.0f) {
                temp->Opacity(temp->Opacity() + Simian::Game::Instance().FrameTime());
                temp->Opacity(Simian::Math::Clamp(temp->Opacity(), 0.0f, 1.0f));
                if (temp) {
                    Simian::f32 val = Simian::Interpolation::Interpolate
                        <Simian::f32>(0, 1.0f, Simian::Math::Clamp(temp->Opacity(), 0.0f, 1.0f),
                        Simian::Interpolation::EaseCubicOut);
                    temp->Scale( 1 + (ICON_START_SCALE-1)*(1-val) );
                    if (skillShow_ == false)
                        temp->Opacity(0.0f);
                }
            } else if (!(bitFlag & 1)) {
                temp->Opacity(0.0f);
            }
            ++i;
        }
    }
    void GCUI::UnloadSkills(bool remove)
    {
        Simian::u32 bitFlag = 31, i = 0; //--check 31 with loadSkill const
        std::stringstream sstr;

        skillShow_ = false;

        if (remove)
        {
            while (bitFlag && i<3)
            {
                sstr.str("");
                sstr << "Icon" << i;
                bitFlag >>= 1;
                if ( (bitFlag) & 1) {
                    Sprite* temp = GetSpritePtr(sstr.str());
                    if (temp)
                        DeleteSprite( temp->SpriteID() );
                }
                ++i;
            }
            uiLoaded_ = false;
        } else {
            while (bitFlag)
            {
                sstr.str("");
                sstr << "Icon" << i;
                bitFlag >>= 1;
                if ( (bitFlag & 1) && i<3) {
                    Sprite* temp = GetSpritePtr(sstr.str());
                    if (temp)
                    {
                        temp->Opacity(0.0f);
                        //temp->Pivot(Simian::Vector2(50, 50));
                    }
                }
                ++i;
            }
        }
    }
    //--------------------------------------------------------------------------
    //--Sprites creation
    bool GCUI::CreateSprite(const std::string& name,
                      Simian::f32 posX,
                      Simian::f32 posY,
                      Simian::f32 width,
                      Simian::f32 height,
                      Simian::f32 scaleX,
                      Simian::f32 scaleY,
                      Simian::f32 rotation,
                      Simian::f32 opacity,
                      Simian::f32 pivotX,
                      Simian::f32 pivotY)
    {
        if (!CheckAvailability())
            return false;

        Sprite temp(name, spriteCount_, 0, rotation, opacity,
            Simian::Vector2(scaleX, scaleY), Simian::Vector2(width, height),
            Simian::Vector2(posX, posY), Simian::Vector2(pivotX, pivotY));
        sprites_.push_back(temp);
        ++spriteCount_;
        return true;
    }
    bool GCUI::DeleteSprite(Simian::u32 id)
    {
        if (id >= spriteCount_) //impossible to delete
            return false;
        //removes sprite from list
        for (Simian::u32 i=id+1; i<spriteCount_; ++i)
            sprites_[i].SpriteID(i-1);
        sprites_.erase(sprites_.begin()+id);
        --spriteCount_;
        return true;
    }
    Sprite& GCUI::GetSprite(const std::string& name)
    {
        Sprite* temp = GetSpritePtr(name);
        if (!temp)
        {
            SAssert(false, "GCUI: Sprites Error: Cannot find by name!");
        }
        return *temp;
    }
    Sprite* GCUI::GetSpritePtr(const std::string& name)
    {
        for (Simian::u32 i=0; i<sprites_.size(); ++i)
        {
            if (sprites_[i].Name() == name)
                return &(sprites_[i]);
        }
        return NULL;
    }
    bool GCUI::CheckAvailability()
    {
        return spriteCount_ < MAX_SPRITE_COUNT;
    }
    //--------------------------------------------------------------------------
    // Draw callback
    void GCUI::AddToRenderQ(Simian::DelegateParameter&)
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif

        Simian::Game::Instance().RenderQueue().AddRenderObject(
            Simian::RenderObject(*material_, Simian::RenderQueue::RL_UI,
            Simian::Matrix::Translation(0, 0, uiDepth_),
            Simian::Delegate::CreateDelegate<GCUI,
            &GCUI::DrawVertexBuffer>(this), 0));
    }
    void GCUI::DrawVertexBuffer(Simian::DelegateParameter&)
    {
        if (!enabled_)
            return;
        vbuffer_->Data(&vertices_[0], (spriteCount_*POLY_VERTEX_COUNT) *
            Simian::PositionColorUVVertex::Format.Size());
        Simian::Game::Instance().GraphicsDevice().DrawIndexed(vbuffer_, ibuffer_,
            Simian::GraphicsDevice::PT_TRIANGLES, spriteCount_*2);
    }
    //--------------------------------------------------------------------------
    void GCUI::Enabled(bool flag)
    {
        enabled_ = flag;
    }
    void GCUI::UIDepth(Simian::f32 val)
    {
        uiDepth_ = val;
    }
    Simian::f32 GCUI::UIDepth()
    {
        return uiDepth_;
    }
    //--------------------------------------------------------------------------
    void GCUI::StartBossBattle(GCAIBase* boss, Simian::u32 bossID)
    {
        bossPtr_ = boss;
        bossID_ = bossID;
        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        CreateSprite("Boss", winSize.X()*0.5f, winSize.Y()*0.5f, //exp
                winSize.Y()*0.5f, winSize.Y()*0.1586f,
                1.0f, 1.0f, 0.0f, 1.0f, winSize.Y()*0.5f*0.5f, winSize.Y()*0.17f*0.5f);
        GetSprite("Boss").UV(&uiFrames[UI_BOSS0]);
        CreateSprite("BossHP",winSize.X()*0.5f-winSize.Y()*0.103f, winSize.Y()*0.441f, //hp
                winSize.Y()*0.293f, winSize.Y()*0.0325f,
                1.0f, 1.0f, 0.0f, 1.0f, winSize.Y()*0.293f*0.5f, winSize.Y()*0.028f);
        GetSprite("BossHP").UV(&uiFrames[UI_HP]);
    }
    void GCUI::EndBossBattle(void)
    {
        if (bossID_ > 0)
        {
            DeleteSprite(GetSprite("Boss").SpriteID());
            DeleteSprite(GetSprite("BossHP").SpriteID());
        }
        bossPtr_ = NULL;
        bossID_ = 0;
    }

    void GCUI::ShowBossName(Simian::f32 inTime, Simian::f32 holdTime,
                            Simian::f32 outTime, Simian::u32 bossENUM)
    {
        bossName_.State = BossNameStruct::BOSSNAME_IN;
        bossName_.InTime = inTime;
        bossName_.HoldTime = holdTime;
        bossName_.OutTime = outTime;
        bossName_.BossENUM = bossENUM;

        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        //--BossName
        Sprite* sprite = GetSpritePtr("BossName");
        if (sprite == NULL)
        {
            CreateSprite("BossName", 0.0f, winSize.Y()*-0.3f,
                winSize.Y()*0.41f, winSize.Y()*0.08f,
                1.0f, 1.0f, 0.0f, 0.0f);
            sprite = GetSpritePtr("BossName");
        }
        sprite->UV(&uiFrames[bossName_.BossENUM]);
        sprite->Pivot(Simian::Vector2(0, -sprite->BoundingBox().Y()));
        if (bossENUM < UI_A0)
            sprite->Scale(Simian::Vector2(1.0f, 1.0f));
        else
            sprite->Scale(Simian::Vector2(1.0f, 0.8f));
    }

    void GCUI::UpdateBossName(void)
    {
        if (bossName_.State != BossNameStruct::BOSSNAME_IDLE)
        {
            Sprite* sprite = GetSpritePtr("BossName");
            if (bossName_.State == BossNameStruct::BOSSNAME_IN)
            {
                sprite->Opacity(sprite->Opacity()+dt_/bossName_.InTime);
                Simian::f32 val = Simian::Interpolation::Interpolate
                    <Simian::f32>(-sprite->BoundingBox().Y(),
                    0, Simian::Math::Clamp<Simian::f32>(sprite->Opacity(),
                    0.0f, 1.0f), Simian::Interpolation::EaseSquareOut);
                sprite->Pivot(Simian::Vector2(sprite->Pivot().X(), val));
                if (sprite->Opacity() >= 1.0f)
                {
                    sprite->Opacity(1.0f);
                    bossName_.State = BossNameStruct::BOSSNAME_HOLD;
                }
            } else if (bossName_.State == BossNameStruct::BOSSNAME_HOLD) {
                bossName_.HoldTime -= dt_;
                if (bossName_.HoldTime <= 0.0f)
                    bossName_.State = BossNameStruct::BOSSNAME_OUT;
            } else if (bossName_.State == BossNameStruct::BOSSNAME_OUT) {
                sprite->Opacity(sprite->Opacity()-dt_/bossName_.OutTime);
                if (sprite->Opacity() <= 0.0f)
                {
                    sprite->Opacity(0.0f);
                    bossName_.State = BossNameStruct::BOSSNAME_IDLE;
                }
            }
        }
    }

    void GCUI::UpdateExpMessage(void)
    {
        GCAttributes* pAttribs = GCPlayerLogic::Instance()->Attributes();
        Sprite* msg0 = GetSpritePtr("EXPMessage0"),
              * msg1 = GetSpritePtr("EXPMessage1");
        static Simian::f32 holdTimer = 0.0f;

        if (!msg0 || !msg1)
            return;

        if (pAttribs->Experience() < pAttribs->ExperienceGoal())
        {
            msg0->Opacity(0.0f);
            msg1->Opacity(0.0f);
            expMsgState_ = EXP_MSG_IDLE;
            return;
        }

        static Simian::f32 inc0 = 0.0f, inc1 = 0.0f;
        if (expMsgState_ == EXP_MSG_IDLE)
            expMsgState_ = EXP_MSG_IN;

        if (expMsgState_ == EXP_MSG_IN)
        {
            holdTimer = 0.0f;
            msg0->Opacity(Simian::Math::Clamp<Simian::f32>(msg0->Opacity() + dt_, 0.0f, 1.0f));
            msg1->Opacity(0.0f);
            if (msg0->Opacity() >= 1.0f)
                expMsgState_ = EXP_MSG_HOLD;
        } else if (expMsgState_ == EXP_MSG_HOLD) {
            holdTimer += dt_;
            if (holdTimer >= 3.0f)
                expMsgState_ = EXP_MSG_SWAP;
        } else if (expMsgState_ == EXP_MSG_SWAP) {
            if (msg0->Opacity() >= 1.0f)
            {
                inc0 = -1.0f;
                inc1 = 1.0f;
            } else {
                inc0 = 1.0f;
                inc1 = -1.0f;
            }
            holdTimer = 0.0f;
            expMsgState_ = EXP_MSG_UPDATESWAP;
        } else if (expMsgState_ == EXP_MSG_UPDATESWAP) {
            msg0->Opacity(Simian::Math::Clamp<Simian::f32>(msg0->Opacity() + dt_*inc0, 0.0f, 1.0f));
            msg1->Opacity(Simian::Math::Clamp<Simian::f32>(msg1->Opacity() + dt_*inc1, 0.0f, 1.0f));
            if (inc0 >= 1.0f && msg0->Opacity() >= 1.0f)
            {
                expMsgState_ = EXP_MSG_HOLD;
            } else if (inc1 >= 1.0f && msg1->Opacity() >= 1.0f) {
                expMsgState_ = EXP_MSG_HOLD;
            }
        }
    }
    //--------------------------------------------------------------------------
    //--Component functions
    void GCUI::Update_(Simian::DelegateParameter& param)
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if (!enabled_)
            return;
        dt_ = param.As<Simian::EntityUpdateParameter>().Dt;
        ResizeAllSprites_();
        UpdateUI_();
        UpdateMenu_();
        UpdateBoss_();
        UpdateVB_();
        UpdateSkills();
        UpdateBossName();
        UpdateExpMessage();
    }
    void GCUI::OnSharedLoad(void)
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::
            CreateDelegate<GCUI, &GCUI::Update_>(this));
        RegisterCallback(Simian::P_RENDER, Simian::Delegate::
            CreateDelegate<GCUI, &GCUI::AddToRenderQ>(this));
        //----------------------------------------------------------------------
        //vertex buffers
        //----------------------------------------------------------------------
        material_ = Simian::Game::Instance().MaterialCache().
            Load("Materials/ui.mat");
        const Simian::Color col(1.0f, 1.0f, 1.0f, 1.0f);

        Simian::Game::Instance().GraphicsDevice().CreateVertexBuffer(vbuffer_);
        Simian::Game::Instance().GraphicsDevice().CreateIndexBuffer(ibuffer_);

        vertices_.resize(MAX_VBSIZE, Simian::PositionColorUVVertex(
            Simian::Vector3(), col, Simian::Vector2()));
        std::vector<Simian::u16> indices;
        for (Simian::u32 i=0; i<(MAX_VBSIZE*3)/2; ++i)
        {
            //012230 BL BR TR TR TL BL
            indices.push_back(static_cast<Simian::u16>(i*4));
            indices.push_back(static_cast<Simian::u16>(i*4+1));
            indices.push_back(static_cast<Simian::u16>(i*4+2));
            indices.push_back(static_cast<Simian::u16>(i*4+2));
            indices.push_back(static_cast<Simian::u16>(i*4+3));
            indices.push_back(static_cast<Simian::u16>(i*4));
        }
        vbuffer_->Load(Simian::PositionColorUVVertex::Format, vertices_.size(), true);
        vbuffer_->Data(&vertices_[0]);
        ibuffer_->Load(Simian::IndexBufferBase::IBT_U16, indices.size());
        ibuffer_->Data(&indices[0]);
    }
    void GCUI::OnSharedUnload(void)
    {
        vbuffer_->Unload();
        ibuffer_->Unload();

        Simian::Game::Instance().GraphicsDevice().DestroyVertexBuffer(vbuffer_);
        Simian::Game::Instance().GraphicsDevice().DestroyIndexBuffer(ibuffer_);
    }
    void GCUI::OnAwake(void)
    {   //reset all
        dt_ = 0.0f;
        menuState_ = bossID_ = spriteCount_ = expMsgState_ = 0;
        pGCUI = this;
        bossPtr_ = NULL;
        enabled_ = true, checkBars_ = true;
        entity_ = NULL;

        sprites_.clear();
        sprites_.reserve(MAX_SPRITE_COUNT);

        //init FSM
        fsm_[MENU_RESTART].OnEnter = Simian::Delegate::CreateDelegate<GCUI,
            &GCUI::MenuRestartEnter_>(this);
        fsm_[MENU_RESTART].OnUpdate = Simian::Delegate::CreateDelegate<GCUI,
            &GCUI::MenuRestartUpdate_>(this);
        fsm_[MENU_RESTART].OnExit = Simian::Delegate::CreateDelegate<GCUI,
            &GCUI::MenuRestartExit_>(this);
        fsm_[MENU_QUIT].OnEnter = Simian::Delegate::CreateDelegate<GCUI,
            &GCUI::MenuQuitEnter_>(this);
        fsm_[MENU_QUIT].OnUpdate = Simian::Delegate::CreateDelegate<GCUI,
            &GCUI::MenuQuitUpdate_>(this);
        fsm_[MENU_QUIT].OnExit = Simian::Delegate::CreateDelegate<GCUI,
            &GCUI::MenuQuitExit_>(this);
        fsm_.ChangeState(MENU_READY);
    }
    void GCUI::OnInit(void)
    {
        //create basic sprites
        gameStatus_ = GAME_RUNNING;
        cinematics_ = GCPlayerLogic::Instance()->PlayerControlOverride();
        Simian::Entity* pEnt = 0;
        pEnt = ParentScene()->EntityByType(E_JUKEBOX);
        if (pEnt)
            pEnt->ComponentByType(Simian::C_JUKEBOX, jukebox_);
        if (uiFrames.size() == 0)
            GenerateUIFrames_();
        Simian::Vector2 winSize(Simian::Game::Instance().Size());
        screenSize_ = winSize;

        LoadUI_();

        //GestureSkills tempSkill = GCGestureInterpreter::Instance()->GetSkills();
        //GCGestureInterpreter::Instance()->SetSkills(GestureSkills(31));
        //LoadSkills();
        //UnloadSkills();
        //GCGestureInterpreter::Instance()->SetSkills(tempSkill);
        LoadSkills();
    }
    void GCUI::OnDeinit(void)
    {
        Simian::Game::Instance().TimeMultiplier(1.0f);
        gameStatus_ = GAME_LOADING;
        //UnloadUI_();
        spriteCount_ = 0;
        vertices_.clear();
        sprites_.clear();
    }
    void GCUI::Serialize(Simian::FileObjectSubNode& data)
    {
        data.AddData("UIDepth", uiDepth_);
    }
    void GCUI::Deserialize(const Simian::FileObjectSubNode& data)
    {
        const Simian::FileDataSubNode* uiDepthNode = data.Data("UIDepth");
        uiDepth_ = uiDepthNode ? uiDepthNode->AsF32() : uiDepth_;
    }
}
