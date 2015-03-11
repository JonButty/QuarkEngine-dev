/*************************************************************************/
/*!
\file		GCUpgradeExchange.cpp
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
#include "Simian/CTransform.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"
#include "Simian/Game.h"
#include "Simian/MaterialCache.h"
#include "Simian/VertexBuffer.h"
#include "Simian/VertexFormat.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/RenderQueue.h"
#include "Simian/IndexBuffer.h"
#include "Simian/CCamera.h"
#include "Simian/Mouse.h"
#include "Simian/Keyboard.h"
#include "Simian/Delegate.h"
#include "Simian/Interpolation.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian/Math.h"
#include "Simian/CModel.h"
#include "Simian/AnimationController.h"
#include "Simian/Math.h"
#include "Simian/CSoundEmitter.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "SaveManager.h"

#include "GCGestureInterpreter.h"
#include "GCGesturePuzzle.h"
#include "GCUpgradeExchange.h"
#include "GCPhysicsController.h"
#include "GCPlayerLogic.h"
#include "GCSpriteEditor.h"
#include "GCUI.h"
#include "GCEntitySpawner.h"
#include "GCEditor.h"
#include "GCDescensionCamera.h"
#include "GCCinematicUI.h"
#include "GCCursor.h"
#include "GCMousePlaneIntersection.h"

#include <iostream>

namespace Descension
{
    const Simian::u32 GCUpgradeExchange::MAX_SPRITE_COUNT = 8,
        GCUpgradeExchange::POLY_VERTEX_COUNT = 4, //quads
        GCUpgradeExchange::MAX_VBSIZE = MAX_SPRITE_COUNT*POLY_VERTEX_COUNT,
        GCUpgradeExchange::ALPHABIT = 24;
    const Simian::f32 GCUpgradeExchange::UV_EPSILON = 0.001f,
        GCUpgradeExchange::ICON_START_SCALE = 4.0f,
        GCUpgradeExchange::ENTER_RAD = 4.0f, GCUpgradeExchange::EXIT_RAD = 5.0f,
        GCUpgradeExchange::RESTART_TIME = 3.0f,
        GCUpgradeExchange::CAM_HEIGHT = 0.2f,
        GCUpgradeExchange::AGGRO_RANGE = 25.0f;
    const Simian::u32 GCUpgradeExchange::KEY_SMASH = Simian::Mouse::KEY_LBUTTON,
        GCUpgradeExchange::KEY_ESC = Simian::Keyboard::KEY_ESCAPE;
    bool GCUpgradeExchange::exchanging_ = false;

    //--------------------------------------------------------------------------
    Simian::FactoryPlant<Simian::EntityComponent, GCUpgradeExchange>
        GCUpgradeExchange::factoryPlant_(&Simian::GameFactory::
        Instance().ComponentFactory(), GC_UPGRADEEXCHANGE);
    GCUpgradeExchange::GCUpgradeExchange()
        : transform_(0), physics_(0), angle_(0), rotationSpeed_(0), vbDepth_(110),
        typeID_(0), spriteCount_(0), smashCount_(0), gemID_(0), prevHP_(0), initHP_(0),
        initExp_(0), timer_(0), dt_(0), floatInc_(0), floatDec_(0), ratio_(0),
        pTimer_(0), winTime_(0), baseScore_(0), maxScore_(0), easeFactor_(0),
        initCost_(0), winCost_(0), shattered_(false), sounds_(0), exchanged_(false),
        spinning_(false), destroyed_(false), material_(0), vbuffer_(0), ibuffer_(0),
        fsm_(UX_STATE_IDLE, UX_STATE_TOTAL)
    {   //--Member init
    }

    //--------------------------------------------------------------------------
    Simian::CTransform& GCUpgradeExchange::Transform()
    {
        return *transform_;
    }
    void GCUpgradeExchange::VBDepth(Simian::f32 val)
    {
        vbDepth_ = val;
    }
    Simian::f32 GCUpgradeExchange::VBDepth(void)
    {
        return vbDepth_;
    }
    bool GCUpgradeExchange::Exchanging(void)
    {
        return exchanging_;
    }
    //--------------------------------------------------------------------------
    void GCUpgradeExchange::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if (GCUI::GameStatus() == GCUI::GAME_PAUSED)
            return;

        dt_ = param.As<Simian::EntityUpdateParameter>().Dt;
        if (!shattered_)
        {
            if (spinning_)
            {
                angle_ += rotationSpeed_*dt_;
                transform_->Rotation(Simian::Vector3(0, -1, 0),
                    Simian::Degree( angle_ ));
            }
        }
        fsm_.Update(dt_);
        ResizeAllSprites_();
        UpdateVB_();
    }
    void GCUpgradeExchange::ResizeAllSprites_(void)
    {
        Simian::Vector2 winSize = Simian::Game::Instance().Size();
        if (std::abs(winSize.X() - screenSize_.X()) < Simian::Math::EPSILON &&
            std::abs(winSize.Y() - screenSize_.Y()) < Simian::Math::EPSILON)
            return;
        //--update all the sprites
        Sprite* sptr = GetSpritePtr("Button");
        if (sptr)
            DeleteSprite(sptr->SpriteID());
        sptr = GetSpritePtr("Window");
        if (sptr)
            DeleteSprite(sptr->SpriteID());
        //--Recreate
        CreateSprite("Window", 0, -winSize.Y()*0.35f,
            winSize.Y()*0.95f, winSize.Y()*0.25f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        GetSprite("Window").UV(&uvFrames_[UX_MSG_HP+typeID_-1]);
        CreateSprite("Button", 0, -winSize.Y()*0.4f,
            winSize.Y()*0.4f, winSize.Y()*0.0535f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        GetSprite("Button").UV(&uvFrames_[UX_START]);
        //--update win size
        screenSize_ = winSize;
    }
    void GCUpgradeExchange::AddParticle_(bool wideArea)
    {
        Simian::Entity* particles = NULL;
        switch (typeID_) {
        case UX_HP:
            particles = ParentEntity()->ParentScene()->CreateEntity(E_REGENHEALTHPARTICLES);
            break;
        case UX_MP:
            particles = ParentEntity()->ParentScene()->CreateEntity(E_REGENMANAPARTICLES);
            break;
        case UX_EXP:
            particles = ParentEntity()->ParentScene()->CreateEntity(E_REGENEXPPARTICLES);
            break;
        }
        if ( particles )
        {
            ParentScene()->AddEntity(particles);
            Simian::CTransform* transferParticlestransform_;
            Simian::CParticleSystem* transferParticles_;
            particles->ComponentByType(Simian::C_TRANSFORM, transferParticlestransform_);
            particles->ComponentByType(Simian::C_PARTICLESYSTEM, transferParticles_);
            if (wideArea)
            {
                const Simian::f32 randomRange = 100.0f;
                transferParticlestransform_->Position(
                    GCPlayerLogic::Instance()->Transform().World().Position() +
                    Simian::Vector3(Simian::Math::Random(0.0f,randomRange)/100.0f-0.5f, 0,
                                    Simian::Math::Random(0.0f,randomRange)/100.0f-0.5f));
            } else {
                transferParticlestransform_->Position(
                    GCPlayerLogic::Instance()->Transform().World().Position());
            }
            transferParticles_->ParticleSystem()->Enabled(true);
        }
    }

    void GCUpgradeExchange::checkDestroyed_(void)
    {
        //--Check if save manager if I'm destroyed
        destroyed_ = SaveManager::Instance().CurrentData().Crystals[gemID_]
                     ? true : false;
        shattered_ = destroyed_;
        if (destroyed_)
            spinning_ = false;
        else
            spinning_ = true;
    }

    void GCUpgradeExchange::updateDestroyed(void)
    {
        //--Update save manager with my destroy status
        SaveManager::Instance().CurrentData().Crystals[gemID_] = 1;
    }

    void GCUpgradeExchange::FSMIdleUpdate_(Simian::DelegateParameter&)
    {
        Simian::Vector3 playerPos = GCPlayerLogic::Instance()->Physics()->Position();

        if (shattered_ || destroyed_)
        {
            GCEntitySpawner::StopEffect(true);
            destroyed_ = true;
            return;
        }

        if ((playerPos-physics_->Position()).LengthSquared() <
            (physics_->Radius()*ENTER_RAD))
        {
            GCEntitySpawner::StopEffect(true);

            bool EnemyFree = true;
            std::vector<GCAIBase*>& enemyVec = GCAIBase::AllEnemiesVec();
            for (Simian::u32 i=0; i<enemyVec.size() && EnemyFree; ++i)
            {
                if (enemyVec[i]->AIType() == GCAIBase::AI_AGGRESSIVE)
                {
                    EnemyFree =
                        ((playerPos-enemyVec[i]->Transform()->
                        WorldPosition()).LengthSquared() >= AGGRO_RANGE);
                }
            }

            if (!EnemyFree)
                return;

            if (GCPlayerLogic::Instance()->Attributes()->Experience() <
                GCPlayerLogic::Instance()->Attributes()->ExperienceGoal())
                GetSprite("Button").UV(&uvFrames_[UX_NOEXP]);
            else
                GetSprite("Button").UV(&uvFrames_[UX_START]);
            Simian::f32 opac = GetSprite("Button").Opacity() + dt_;
            GetSprite("Button").Opacity(Simian::Math::Clamp<Simian::f32>
                (opac, 0.0f, 1.0f));
            if (Simian::Mouse::IsTriggered(KEY_SMASH) &&
                GCPlayerLogic::Instance()->Attributes()->Experience() >=
                GCPlayerLogic::Instance()->Attributes()->ExperienceGoal())
            {
                fsm_.ChangeState(UX_STATE_INTRO);
                GetSprite("Button").Opacity(0.0f);
                prevHP_ = GCPlayerLogic::Instance()->Attributes()->Health();
            }
        } else {
            Simian::f32 opac = GetSprite("Button").Opacity() - dt_;
            GetSprite("Button").Opacity(Simian::Math::Clamp<Simian::f32>
                (opac, 0.0f, 1.0f));
            GCEntitySpawner::StopEffect(false);
        }
    }
    void GCUpgradeExchange::FSMIntroUpdate_(Simian::DelegateParameter&)
    {
        Simian::Vector3 playerPos = GCPlayerLogic::Instance()->Physics()->Position();
        exchanging_ = true;

        GetSprite("Window").UV(&uvFrames_[UX_MSG_HP+typeID_-1]);
        Simian::f32 opac = GetSprite("Window").Opacity() + dt_;
        GetSprite("Window").Opacity(Simian::Math::Clamp<Simian::f32>
            (opac, 0.0f, 1.0f));
        GCEntitySpawner::StopEffect(true);

        if (Simian::Mouse::IsTriggered(KEY_SMASH))
            fsm_.ChangeState(UX_STATE_START);

        bool EnemyFree = true;
        std::vector<GCAIBase*>& enemyVec = GCAIBase::AllEnemiesVec();
        for (Simian::u32 i=0; i<enemyVec.size() && EnemyFree; ++i)
        {
            if (enemyVec[i]->AIType() == GCAIBase::AI_AGGRESSIVE)
            {
                EnemyFree =
                    ((playerPos-enemyVec[i]->Transform()->
                    WorldPosition()).LengthSquared() >= AGGRO_RANGE);
            }
        }

        if (Simian::Keyboard::IsTriggered(KEY_ESC) || !EnemyFree ||
            (playerPos-physics_->Position()).LengthSquared() >
            (physics_->Radius()*EXIT_RAD)*(physics_->Radius()*EXIT_RAD) ||
            prevHP_ > GCPlayerLogic::Instance()->Attributes()->Health() )
        {
            exchanging_ = false;
            fsm_.ChangeState(UX_STATE_END);
            GetSprite("Button").UV(&uvFrames_[UX_INTERRUPTED]);
            GetSprite("Button").Opacity(1.0f);
            GetSprite("Window").Opacity(0.0f);
        }
    }
    void GCUpgradeExchange::FSMStartEnter_(Simian::DelegateParameter&)
    {
        smashCount_ = 0;
        floatInc_ = floatDec_ = timer_ = pTimer_ = 0.0f;

        GCEntitySpawner::StopEffect(false);
        prevHP_ = GCPlayerLogic::Instance()->Attributes()->Health();
        initExp_ = GCPlayerLogic::Instance()->Attributes()->Experience();
        
        switch (typeID_)
        {
            case UX_HP:
                ratio_ = static_cast<Simian::f32>(GCPlayerLogic::Instance()->Attributes()->Health())/
                    GCPlayerLogic::Instance()->Attributes()->MaxHealth();
                initHP_ = GCPlayerLogic::Instance()->Attributes()->MaxHealth();
                break;
            case UX_MP:
                ratio_ = static_cast<Simian::f32>(GCPlayerLogic::Instance()->Attributes()->Mana())/
                    GCPlayerLogic::Instance()->Attributes()->MaxMana();
                initHP_ = (Simian::s32)GCPlayerLogic::Instance()->Attributes()->MaxMana();
                break;
            case UX_EXP:
                initHP_ = GCPlayerLogic::Instance()->Attributes()->Experience();
                break;
        }

        GetSprite("Button").UV(&uvFrames_[UX_ESCAPE]);
        GetSprite("Button").Opacity(0.0f);
        GetSprite("Window").Opacity(0.0f);

        if (typeID_ != UX_EXP)
        {
            if (GCPlayerLogic::Instance()->Attributes()->Experience() <
                static_cast<Simian::u32>(initCost_*
                GCPlayerLogic::Instance()->Attributes()->ExperienceGoal()))
            {   //--no cost
                GetSprite("Button").Opacity(1.0f);
                GetSprite("Button").UV(&uvFrames_[UX_NOEXP]);
                smashCount_ = 978379; //hax
            }// else {
                //GCPlayerLogic::Instance()->Attributes()->Experience(initExp_ - 
                    //(Simian::u32)initCost_ );
                //initExp_ -= (Simian::u32)initCost_;
            //}
        } else if (GCPlayerLogic::Instance()->Attributes()->Experience() >= 
                   GCPlayerLogic::Instance()->Attributes()->ExperienceGoal()) {
            GetSprite("Button").Opacity(1.0f);
            GetSprite("Button").UV(&uvFrames_[UX_FULL]);
            smashCount_ = 978379; //hax
        }

        GCCursor::Instance()->ShowBoth(true);
        GCCursor::Instance()->Spirit()->Active(false);

        GCPlayerLogic::Instance()->CameraLocked(false);
        GCDescensionCamera::Instance()->Interpolate(GCPlayerLogic::Instance()->
            Transform().World().Position() + Simian::Vector3(0, CAM_HEIGHT, 0),
            yawPitchZoom_, winTime_, Simian::Interpolation::EaseCubicInOut);
    }
    void GCUpgradeExchange::FSMStartUpdate_(Simian::DelegateParameter&)
    {
        static const Simian::f32 pTimeReset = 0.5f;
        static bool frameSwitch = false;
        Simian::Vector3 playerPos = GCPlayerLogic::Instance()->Physics()->Position();
        Simian::u32 adder = 0;

        if (smashCount_ == 978379) //hax
        {
            fsm_.ChangeState(UX_STATE_END);
            return;
        }

        exchanged_ = true;
        timer_ += dt_;
        pTimer_ += dt_;

        Simian::f32 opac = GetSprite("Button").Opacity() + dt_;
        GetSprite("Button").Opacity(Simian::Math::Clamp<Simian::f32>
            (opac, 0.0f, 1.0f));

        if (Simian::Mouse::IsTriggered(KEY_SMASH))
        {   //--Ease to upper bonus
            floatInc_ += ((maxScore_-baseScore_) - floatInc_) * easeFactor_;
            ++smashCount_;
        }

        adder = static_cast<Simian::u32>( //--integer casted total of base+bonus
            std::floor(floatInc_ + timer_/winTime_*baseScore_) );
        GCAttributes* attrib;
        GCPlayerLogic::Instance()->ComponentByType(GC_ATTRIBUTES, attrib);

        if (pTimer_ >= pTimeReset)
        {
            pTimer_ -= pTimeReset;
            AddParticle_();
            frameSwitch = !frameSwitch;
            if (frameSwitch)
                GetSprite("Button").UV(&uvFrames_[UX_ESCAPE2]);
            else
                GetSprite("Button").UV(&uvFrames_[UX_ESCAPE]);
        }

        if (prevHP_ > GCPlayerLogic::Instance()->Attributes()->Health())
        {
            fsm_.ChangeState(UX_STATE_END);
            GetSprite("Button").UV(&uvFrames_[UX_INTERRUPTED]);
            return;
        }

        if (attrib->Experience() <= 0)
        {
            fsm_.ChangeState(UX_STATE_END);
            GetSprite("Button").UV(&uvFrames_[UX_COMPLETED]);
            return;
        }

        switch (typeID_) {
        case UX_HP:
            attrib->MaxHealth(initHP_ + adder);
            attrib->Health((Simian::s32)(attrib->MaxHealth()*ratio_));
            if (attrib->MaxHealth()>=attrib->MaxMaxHealth())
            {
                fsm_.ChangeState(UX_STATE_END);
                GetSprite("Button").UV(&uvFrames_[UX_FULL]);
            }
            break;
        case UX_MP:
            attrib->MaxMana((Simian::f32)initHP_+ adder);
            attrib->Mana(attrib->MaxMana()*ratio_);
            if (attrib->MaxMana()>=attrib->MaxMaxMana())
            {
                fsm_.ChangeState(UX_STATE_END);
                GetSprite("Button").UV(&uvFrames_[UX_FULL]);
            }
            break;
        case UX_EXP:
            attrib->Experience(Simian::Math::Min(initHP_ + adder,
                               attrib->ExperienceGoal()));
            break;
        }

        floatDec_ += winCost_*attrib->ExperienceGoal()*(dt_/winTime_);
        adder = static_cast<Simian::u32>(std::floor(floatDec_));

        if (/*Simian::Keyboard::IsTriggered(KEY_ESC) ||*/
            (playerPos-physics_->Position()).LengthSquared() >
            (physics_->Radius()*EXIT_RAD)*(physics_->Radius()*EXIT_RAD))
        {
            fsm_.ChangeState(UX_STATE_END);
            GetSprite("Button").UV(&uvFrames_[UX_INTERRUPTED]);
            return;
        }

        if (timer_ >= winTime_)
        {
            fsm_.ChangeState(UX_STATE_END);
            GetSprite("Button").UV(&uvFrames_[UX_COMPLETED]);
        }

        prevHP_ = GCPlayerLogic::Instance()->Attributes()->Health();
        if (typeID_ != UX_EXP)
            attrib->Experience(initExp_ - adder);
    }
    void GCUpgradeExchange::FSMEndEnter_(Simian::DelegateParameter&)
    {
        floatInc_ = 0.0f;
        GetSprite("Button").Opacity(1.0f);
        Simian::Vector3 yawPitchZoom =
            GCDescensionCamera::Instance()->StartingYawPitchZoom() -
            GCDescensionCamera::Instance()->YawPitchZoom();
        GCDescensionCamera::Instance()->Interpolate(GCPlayerLogic::Instance()->
            Transform().World().Position(),
            yawPitchZoom, 0.75f, Simian::Interpolation::EaseSquareInOut);
        GCPlayerLogic::Instance()->CameraLocked(true);

        if (timer_ >= winTime_)
        {
            for(Simian::u32 i=0; i<MAX_SPRITE_COUNT*POLY_VERTEX_COUNT; ++i)
                AddParticle_(true);
        }
        timer_ = 0.0f;

        GCCursor::Instance()->ShowBoth(false);
        GCCursor::Instance()->Spirit()->Active(true);
        GCCursor::Instance()->Spirit()->InterpolateToPos(true);

        if (exchanged_)
        {
            updateDestroyed();
            spinning_ = false;
            Simian::CModel* model;
            ComponentByType(Simian::C_MODEL, model);
            model->Controller().PlayAnimation("Break", false);
            if (sounds_)
                sounds_->Play(0);
        }
    }

    void GCUpgradeExchange::FSMEndUpdate_(Simian::DelegateParameter&)
    {
        exchanging_ = false;
        floatInc_ += dt_;
        GCEntitySpawner::StopEffect(true);
        if (RESTART_TIME - floatInc_ < 1.0f)
        {
            Simian::f32 opac = GetSprite("Button").Opacity() - dt_;
            GetSprite("Button").Opacity(Simian::Math::Clamp<Simian::f32>
                (opac, 0.0f, 1.0f));
        }
        if (floatInc_ < RESTART_TIME)
            return;
        fsm_.ChangeState(UX_STATE_IDLE);
        if (exchanged_)
            shattered_ = true;
    }
    //--------------------------------------------------------------------------
    void GCUpgradeExchange::AddToRenderQ(Simian::DelegateParameter&)
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if (spriteCount_ == 0)
            return;
        Simian::Game::Instance().RenderQueue().AddRenderObject(
            Simian::RenderObject(*material_, Simian::RenderQueue::RL_UI,
            Simian::Matrix::Translation(0, 0, vbDepth_),
            Simian::Delegate::CreateDelegate<GCUpgradeExchange,
            &GCUpgradeExchange::DrawVertexBuffer>(this), 0));
    }
    void GCUpgradeExchange::UpdateVB_(void)
    {
        for (Simian::u32 i=0; i<spriteCount_; ++i)
        {
            Simian::u32 j = i*4;
            GCUI::Hexa32 newAlp(vertices_[j].Diffuse);
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
    void GCUpgradeExchange::DrawVertexBuffer(Simian::DelegateParameter&)
    {
        vbuffer_->Data(&vertices_[0], (spriteCount_*POLY_VERTEX_COUNT) *
            Simian::PositionColorUVVertex::Format.Size());
        Simian::Game::Instance().GraphicsDevice().DrawIndexed(vbuffer_,
            ibuffer_, Simian::GraphicsDevice::PT_TRIANGLES, spriteCount_*2);
    }
    //--------------------------------------------------------------------------
    //--Sprites creation
    bool GCUpgradeExchange::CreateSprite(const std::string& name,
        Simian::f32 posX, Simian::f32 posY, Simian::f32 width, Simian::f32 height,
        Simian::f32 scaleX, Simian::f32 scaleY, Simian::f32 rotation,
        Simian::f32 opacity, Simian::f32 pivotX, Simian::f32 pivotY)
    {
        if (!CheckAvailability())
            return false;
        Sprite::CreateSprite(sprites_, spriteCount_, name, posX, posY, width,
            height, scaleX, scaleY, rotation, opacity, pivotX, pivotY);
        ++spriteCount_;
        return true;
    }
    bool GCUpgradeExchange::DeleteSprite(Simian::u32 id)
    {
        if (id >= spriteCount_) //impossible to delete
            return false;
        //removes sprite from list
        Sprite::DeleteSprite(sprites_, id);
        --spriteCount_;
        return true;
    }
    Sprite& GCUpgradeExchange::GetSprite(const std::string& name)
    {
        return Sprite::GetSprite(sprites_, name);
    }
    Sprite* GCUpgradeExchange::GetSpritePtr(const std::string& name)
    {
        return Sprite::GetSpritePtr(sprites_, name);
    }
    bool GCUpgradeExchange::CheckAvailability()
    {
        return spriteCount_ < MAX_SPRITE_COUNT;
    }
    void GCUpgradeExchange::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate
            <GCUpgradeExchange, &GCUpgradeExchange::update_>(this));
        RegisterCallback(Simian::P_RENDER, Simian::Delegate::
            CreateDelegate<GCUpgradeExchange,
            &GCUpgradeExchange::AddToRenderQ>(this));

        material_ = Simian::Game::Instance().MaterialCache().
            Load("Materials/UpgradeExchange.mat");
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

    void GCUpgradeExchange::OnSharedUnload(void)
    {
        vbuffer_->Unload();
        ibuffer_->Unload();

        Simian::Game::Instance().GraphicsDevice().DestroyVertexBuffer(vbuffer_);
        Simian::Game::Instance().GraphicsDevice().DestroyIndexBuffer(ibuffer_);
    }

    void GCUpgradeExchange::OnAwake()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        spriteCount_ = 0;
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);
        ComponentByType(GC_PHYSICSCONTROLLER, physics_);
        exchanging_ = false;
        prevHP_ = 0;

        fsm_[UX_STATE_IDLE].OnUpdate = Simian::Delegate::CreateDelegate<GCUpgradeExchange,
            &GCUpgradeExchange::FSMIdleUpdate_>(this);
        fsm_[UX_STATE_INTRO].OnUpdate = Simian::Delegate::CreateDelegate<GCUpgradeExchange,
            &GCUpgradeExchange::FSMIntroUpdate_>(this);
        fsm_[UX_STATE_START].OnEnter = Simian::Delegate::CreateDelegate<GCUpgradeExchange,
            &GCUpgradeExchange::FSMStartEnter_>(this);
        fsm_[UX_STATE_START].OnUpdate = Simian::Delegate::CreateDelegate<GCUpgradeExchange,
            &GCUpgradeExchange::FSMStartUpdate_>(this);
        fsm_[UX_STATE_END].OnEnter = Simian::Delegate::CreateDelegate<GCUpgradeExchange,
            &GCUpgradeExchange::FSMEndEnter_>(this);
        fsm_[UX_STATE_END].OnUpdate = Simian::Delegate::CreateDelegate<GCUpgradeExchange,
            &GCUpgradeExchange::FSMEndUpdate_>(this);

        fsm_.ChangeState(UX_STATE_IDLE);
    }

    void GCUpgradeExchange::OnInit()
    {
        GCSpriteEditor::GetUVFromFile("UI/UV_UpgradeExchange.xml", uvFrames_,
                                       UX_TOTAL_FRAMES);
        ResizeAllSprites_();
        shattered_ = exchanged_ = false;
        spinning_ = true;

        checkDestroyed_();
        if (destroyed_)
        {
            Simian::CModel* model;
            ComponentByType(Simian::C_MODEL, model);
            model->Controller().PlayAnimation("Break", false);
        }
    }

    void GCUpgradeExchange::OnTriggered(Simian::DelegateParameter&)
    {
        //--nothing
    }

    void GCUpgradeExchange::OnDeinit(void)
    {
        spriteCount_ = 0;
        uvFrames_.clear();
        vertices_.clear();
        sprites_.clear();
    }

    void GCUpgradeExchange::Serialize( Simian::FileObjectSubNode& data )
    {
        std::string output;
        switch (typeID_) {
        case UX_HP:
            output = "HP"; break;
        case UX_MP:
            output = "MP"; break;
        case UX_EXP:
            output = "EXP"; break;
        default:
            output = "NONE"; break;
        }
        data.AddData("UpgradeType", output.c_str());
        data.AddData("RotationSpeed", rotationSpeed_);
        data.AddData("WinTime", winTime_);
        data.AddData("BaseScore", baseScore_);
        data.AddData("MaxScore", maxScore_);
        data.AddData("EaseFactor", easeFactor_);
        data.AddData("InitCost", initCost_);
        data.AddData("WinCost", winCost_);
        data.AddData("GemID", gemID_);

        data.AddData("Yaw", yawPitchZoom_.X());
        data.AddData("Pitch", yawPitchZoom_.Y());
        data.AddData("Zoom", yawPitchZoom_.Z());
    }

    void GCUpgradeExchange::Deserialize( const Simian::FileObjectSubNode& data)
    {
        const Simian::FileDataSubNode* node = data.Data("UpgradeType");
        if (node)
        {
            std::string input = node->AsString();
            if (input == "HP")
                typeID_ = UX_HP;
            else if (input == "MP")
                typeID_ = UX_MP;
            else if (input == "EXP")
                typeID_ = UX_EXP;
            else
                typeID_ = UX_NONE;
        }
        data.Data("RotationSpeed", rotationSpeed_, rotationSpeed_);
        data.Data("WinTime", winTime_, winTime_);
        data.Data("BaseScore", baseScore_, baseScore_);
        data.Data("MaxScore", maxScore_, maxScore_);
        data.Data("EaseFactor", easeFactor_, easeFactor_);
        data.Data("InitCost", initCost_, initCost_);
        data.Data("WinCost", winCost_, winCost_);
        data.Data("GemID", gemID_, gemID_);

        Simian::f32 x=0, y=0, z=0;
        data.Data("Yaw", x, x);
        data.Data("Pitch", y, y);
        data.Data("Zoom", z, z);
        yawPitchZoom_ = Simian::Vector3(x, y ,z);
    }
}
