/*************************************************************************/
/*!
\file		GCGesturePuzzleTrigger.cpp
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
#include "Simian/CSoundEmitter.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"

#include "GCGesturePuzzleTrigger.h"
#include "GCPhysicsController.h"
#include "GCGesturePuzzle.h"
#include "GCPlayerLogic.h"
#include "GCSpriteEditor.h"
#include "GCEditor.h"
#include "GCUI.h"

#include <iostream>

namespace Descension
{
    const Simian::u32 GCGesturePuzzleTrigger::MAX_SPRITE_COUNT = 32,
        GCGesturePuzzleTrigger::POLY_VERTEX_COUNT = 4, //quads
        GCGesturePuzzleTrigger::MAX_VBSIZE = MAX_SPRITE_COUNT*POLY_VERTEX_COUNT,
        GCGesturePuzzleTrigger::ALPHABIT = 24;
    const Simian::f32 GCGesturePuzzleTrigger::UV_EPSILON = 0.001f,
        GCGesturePuzzleTrigger::ICON_START_SCALE = 4.0f,
        GCGesturePuzzleTrigger::FADE_SPEED = 3.0f;

    GCGesturePuzzleTrigger* GCGesturePuzzleTrigger::instance_ = NULL;
    //--------------------------------------------------------------------------
    Simian::FactoryPlant<Simian::EntityComponent, GCGesturePuzzleTrigger>
        GCGesturePuzzleTrigger::factoryPlant_(&Simian::GameFactory::
        Instance().ComponentFactory(), GC_GESTUREPUZZLETRIGGER);
    GCGesturePuzzleTrigger::GCGesturePuzzleTrigger()
        : transform_(0), physics_(0), vbDepth_(110), timer_(0), dt_(0),
        puzzleID_(0), spriteCount_(0), sounds_(0), soundPlayed_(false), material_(0), vbuffer_(0),
        ibuffer_(0), fsm_(IDLE, INNERSTATE_TOTAL)
    { }
    //--------------------------------------------------------------------------
    Simian::CTransform& GCGesturePuzzleTrigger::Transform()
    {
        return *transform_;
    }
    void GCGesturePuzzleTrigger::VBDepth(Simian::f32 val)
    {
        vbDepth_ = val;
    }
    Simian::f32 GCGesturePuzzleTrigger::VBDepth(void)
    {
        return vbDepth_;
    }

    bool GCGesturePuzzleTrigger::PlayerInRange(void)
    {
        if (instance_ == NULL)
            return false;
        return instance_->fsm_.CurrentState() == NEARBY;
    }
    //--------------------------------------------------------------------------
    void GCGesturePuzzleTrigger::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        dt_ = param.As<Simian::EntityUpdateParameter>().Dt;

        fsm_.Update(dt_);
        ResizeAllSprites_();
        UpdateVB_();
    }
    void GCGesturePuzzleTrigger::ResizeAllSprites_(void)
    {
        Simian::Vector2 winSize = Simian::Game::Instance().Size();
        if (std::abs(winSize.X() - screenSize_.X()) < Simian::Math::EPSILON &&
            std::abs(winSize.Y() - screenSize_.Y()) < Simian::Math::EPSILON)
            return;
        //--update all the sprites
        Sprite* button = GetSpritePtr("Button");
        Sprite* hint = GetSpritePtr("Window");
        if (button)
            DeleteSprite(button->SpriteID());
        if (hint)
            DeleteSprite(hint->SpriteID());
        CreateSprite("Window", 0, winSize.Y()*0.075f,
        winSize.Y()*0.75f, winSize.Y()*0.8f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
        GetSprite("Window").UV(&uvFrames_[GPUZZLE_FRAME_WINDOW]);
        CreateSprite("Button", 0, -winSize.Y()*0.39f,
            winSize.Y()*0.5f, winSize.Y()*0.125f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
        GetSprite("Button").UV(&uvFrames_[GPUZZLE_FRAME_BUTTON]);
        //--update win size
        screenSize_ = winSize;
    }

    void GCGesturePuzzleTrigger::FSMIdleUpdate_(Simian::DelegateParameter&)
    {
        static const Simian::f32 enterR = 3.0f;

        Simian::Vector3 player = GCPlayerLogic::Instance()->Physics()->Position();
        if ((player - physics_->Position()).LengthSquared() <
            physics_->Radius()*enterR)
        {
            fsm_.ChangeState(NEARBY);
            return;
        }

        Simian::f32 opac = GetSprite("Window").Opacity() - dt_*FADE_SPEED;
        GetSprite("Window").Opacity(Simian::Math::Clamp<Simian::f32>
            (opac, 0.0f, 1.0f));
        opac = GetSprite("Button").Opacity() - dt_;
        GetSprite("Button").Opacity(Simian::Math::Clamp<Simian::f32>(opac, 0.0f, 1.0f));

    }
    void GCGesturePuzzleTrigger::FSMNearbyUpdate_(Simian::DelegateParameter&)
    {
        static const Simian::f32 exitR = 5.0f;

        Simian::f32 opac = GetSprite("Button").Opacity() + dt_*FADE_SPEED;
        GetSprite("Button").Opacity(Simian::Math::Clamp<Simian::f32>(opac, 0.0f, 1.0f));

        Simian::Vector3 player = GCPlayerLogic::Instance()->Physics()->Position();
        if ((player - physics_->Position()).LengthSquared() >
            physics_->Radius() * exitR)
        {
            fsm_.ChangeState(IDLE);
            return;
        }

        if (Simian::Mouse::IsPressed(Simian::Mouse::KEY_LBUTTON))
        {
            opac = GetSprite("Window").Opacity() + dt_*FADE_SPEED;
            GetSprite("Window").Opacity(Simian::Math::Clamp<Simian::f32>
                (opac, 0.0f, 1.0f));
            GCGesturePuzzle::UnlockPuzzle(puzzleID_);
            if (!soundPlayed_ && sounds_)
            {
                soundPlayed_ = true;
                sounds_->Play(0);
            }
        } else {
            opac = GetSprite("Window").Opacity() - dt_*FADE_SPEED;
            GetSprite("Window").Opacity(Simian::Math::Clamp<Simian::f32>
                (opac, 0.0f, 1.0f));
            soundPlayed_ = false;
        }
    }
    //--------------------------------------------------------------------------
    void GCGesturePuzzleTrigger::UpdateVB_(void)
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
    void GCGesturePuzzleTrigger::AddToRenderQ(Simian::DelegateParameter&)
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        Simian::Game::Instance().RenderQueue().AddRenderObject(
            Simian::RenderObject(*material_, Simian::RenderQueue::RL_UI,
            Simian::Matrix::Translation(0, 0, vbDepth_),
            Simian::Delegate::CreateDelegate<GCGesturePuzzleTrigger,
            &GCGesturePuzzleTrigger::DrawVertexBuffer>(this), 0));
    }
    void GCGesturePuzzleTrigger::DrawVertexBuffer(Simian::DelegateParameter&)
    {
        vbuffer_->Data(&vertices_[0], (spriteCount_*POLY_VERTEX_COUNT) *
            Simian::PositionColorUVVertex::Format.Size());
        Simian::Game::Instance().GraphicsDevice().DrawIndexed(vbuffer_,
            ibuffer_, Simian::GraphicsDevice::PT_TRIANGLES, spriteCount_*2);
    }
    //--------------------------------------------------------------------------
    //--Sprites creation
    bool GCGesturePuzzleTrigger::CreateSprite(const std::string& name,
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
    bool GCGesturePuzzleTrigger::DeleteSprite(Simian::u32 id)
    {
        if (id >= spriteCount_) //impossible to delete
            return false;
        //removes sprite from list
        Sprite::DeleteSprite(sprites_, id);
        --spriteCount_;
        return true;
    }
    Sprite& GCGesturePuzzleTrigger::GetSprite(const std::string& name)
    {
        return Sprite::GetSprite(sprites_, name);
    }
    Sprite* GCGesturePuzzleTrigger::GetSpritePtr(const std::string& name)
    {
        return Sprite::GetSpritePtr(sprites_, name);
    }
    bool GCGesturePuzzleTrigger::CheckAvailability()
    {
        return spriteCount_ < MAX_SPRITE_COUNT;
    }
    void GCGesturePuzzleTrigger::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate
            <GCGesturePuzzleTrigger, &GCGesturePuzzleTrigger::update_>(this));
        RegisterCallback(Simian::P_RENDER, Simian::Delegate::
            CreateDelegate<GCGesturePuzzleTrigger,
            &GCGesturePuzzleTrigger::AddToRenderQ>(this));

        material_ = Simian::Game::Instance().MaterialCache().
            Load(texture_);
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

    void GCGesturePuzzleTrigger::OnSharedUnload(void)
    {
        vbuffer_->Unload();
        ibuffer_->Unload();

        Simian::Game::Instance().GraphicsDevice().DestroyVertexBuffer(vbuffer_);
        Simian::Game::Instance().GraphicsDevice().DestroyIndexBuffer(ibuffer_);
    }

    void GCGesturePuzzleTrigger::OnAwake()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        spriteCount_ = 0;
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(GC_PHYSICSCONTROLLER, physics_);
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);

        fsm_[IDLE].OnUpdate = Simian::Delegate::CreateDelegate<GCGesturePuzzleTrigger,
            &GCGesturePuzzleTrigger::FSMIdleUpdate_>(this);
        fsm_[NEARBY].OnUpdate = Simian::Delegate::CreateDelegate<GCGesturePuzzleTrigger,
            &GCGesturePuzzleTrigger::FSMNearbyUpdate_>(this);

        fsm_.ChangeState(IDLE);
    }

    void GCGesturePuzzleTrigger::OnInit()
    {
        GCSpriteEditor::GetUVFromFile("UI/UV_GesturePuzzleTrigger.xml", uvFrames_,
            GPUZZLE_FRAME_TOTAL);
        instance_ = this;
        ResizeAllSprites_();
        GetSprite("Button").Opacity(0.0f);
        GetSprite("Window").Opacity(0.0f);
    }

    void GCGesturePuzzleTrigger::OnTriggered(Simian::DelegateParameter&)
    {
        //--script trigger
    }

    void GCGesturePuzzleTrigger::OnDeinit(void)
    {
        //Simian::Game::Instance().TimeMultiplier(1.0f);
        DeleteSprite(0);
        DeleteSprite(1);
        spriteCount_ = 0;
        uvFrames_.clear();
        vertices_.clear();
        sprites_.clear();

        instance_ = NULL;
    }

    void GCGesturePuzzleTrigger::Serialize( Simian::FileObjectSubNode& data)
    {
        data.AddData("GestureID", puzzleID_);
        data.AddData("Texture", texture_);
    }

    void GCGesturePuzzleTrigger::Deserialize( const Simian::FileObjectSubNode& data)
    {
        const Simian::FileDataSubNode* gestureID = data.Data("GestureID");
        if (gestureID)
            puzzleID_ = gestureID->AsU32();
        const Simian::FileDataSubNode* texture = data.Data("Texture");
        if (texture)
            texture_ = texture->AsString();
    }
}
