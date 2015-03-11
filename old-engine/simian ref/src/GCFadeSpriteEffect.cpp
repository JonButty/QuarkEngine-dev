/******************************************************************************/
/*!
\file		GCFadeSpriteEffect.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/Interpolation.h"
#include "Simian/CSprite.h"
#include "Simian/Math.h"
#include "Simian/Color.h"

#include "GCFadeSpriteEffect.h"
#include "ComponentTypes.h"
#include "Simian/LogManager.h"
#include "Simian/EnginePhases.h"
#include "GCEditor.h"

static const Simian::Color START;
static const Simian::Color END(1.0f,1.0f,1.0f,0.0f);

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCFadeSprite> GCFadeSprite::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_FADESPRITEEFFECT);

    GCFadeSprite::GCFadeSprite()
        :   enable_(true),
            autoStart_(true),
            start_(true),
            fadeIn_(false),
            sprite_(0),
            idleTime_(0),
            fadeTime_(0),
            timer_(0),
            fsm_(FS_IDLE,FS_TOTAL)
    {
    }

    GCFadeSprite::~GCFadeSprite()
    {

    }

    //--------------------------------------------------------------------------

    bool GCFadeSprite::AutoStart() const
    {
        return autoStart_;
    }

    void GCFadeSprite::AutoStart( bool val )
    {
        autoStart_ = val;
        start_ = autoStart_;
    }

    bool GCFadeSprite::Start() const
    {
        return start_;
    }

    void GCFadeSprite::Start( bool val )
    {
        start_ = val;
    }

    bool GCFadeSprite::FadeIn() const
    {
        return fadeIn_;
    }

    void GCFadeSprite::FadeIn( bool val )
    {
        fadeIn_ = val;
    }

    void GCFadeSprite::ForceEnd()
    {
        timer_ = fadeTime_;
        sprite_->Color(Simian::Interpolation::Interpolate
            <Simian::Color>(START,END,1.0f,Simian::Interpolation::EaseSquareOut));
        sprite_->Visible(false);
    }

    Simian::f32 GCFadeSprite::IdleTime() const
    {
        return idleTime_;
    }

    void GCFadeSprite::IdleTime( Simian::f32 val )
    {
        idleTime_ = val;
    }

    Simian::f32 GCFadeSprite::FadeTime() const
    {
        return fadeTime_;
    }

    void GCFadeSprite::FadeTime( Simian::f32 val )
    {
        fadeTime_ = val;
    }

    //--------------------------------------------------------------------------

    void GCFadeSprite::Reset()
    {
        fsm_.ChangeState(FS_IDLE);
        start_ = autoStart_;
        timer_ = 0.0f;
        sprite_->Visible(true);
        sprite_->Color(Simian::Interpolation::Interpolate
            <Simian::Color>(START,END,timer_,Simian::Interpolation::EaseSquareOut));
    }

    void GCFadeSprite::OnSharedLoad()
    {

    }

    void GCFadeSprite::OnAwake()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCFadeSprite, &GCFadeSprite::update_>(this));
    }

    void GCFadeSprite::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        ParentEntity()->ComponentByType(Simian::C_SPRITE,sprite_);
        if(!sprite_)
            enable_ = false;

        fsm_[FS_IDLE].OnEnter = Simian::Delegate::CreateDelegate<GCFadeSprite, &GCFadeSprite::idleOnEnter_>(this);
        fsm_[FS_IDLE].OnUpdate = Simian::Delegate::CreateDelegate<GCFadeSprite, &GCFadeSprite::idleOnUpdate_>(this);
        fsm_[FS_FADE].OnEnter = Simian::Delegate::CreateDelegate<GCFadeSprite, &GCFadeSprite::fadeOnEnter_>(this);
        fsm_[FS_FADE].OnUpdate = Simian::Delegate::CreateDelegate<GCFadeSprite, &GCFadeSprite::fadeOnUpdate_>(this);
    }

    void GCFadeSprite::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("AutoStart",autoStart_);
        data.AddData("IdleTime",idleTime_);
        data.AddData("FadeTime",fadeTime_);
    }

    void GCFadeSprite::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("AutoStart",autoStart_,autoStart_);
        start_ = autoStart_;
        data.Data("IdleTime",idleTime_,idleTime_);
        data.Data("FadeTime",fadeTime_,fadeTime_);
    }

    //-Private------------------------------------------------------------------

    void GCFadeSprite::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if(!enable_ || !start_)
            return;

        fsm_.Update(param.As<Simian::EntityUpdateParameter>().Dt);
    }

    void GCFadeSprite::idleOnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = 0.0f;
    }

    void GCFadeSprite::idleOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;
        if(timer_ > idleTime_)
            fsm_.ChangeState(FS_FADE);

        if (fadeIn_ && sprite_->Color().A() < 1.0f)
        {
            sprite_->Visible(true);
            Simian::Color col = sprite_->Color();
            col.A(col.A() + param.As<Simian::EntityUpdateParameter>().Dt);
        }
    }

    void GCFadeSprite::fadeOnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = 0.0f;
        fadeIn_ = false;
    }

    void GCFadeSprite::fadeOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;

        Simian::f32 intp = timer_/fadeTime_;
        intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
        sprite_->Color(Simian::Interpolation::Interpolate
            <Simian::Color>(START,END,intp,Simian::Interpolation::EaseSquareOut));
        if (timer_ > fadeTime_)
            sprite_->Visible(false);
    }
}
