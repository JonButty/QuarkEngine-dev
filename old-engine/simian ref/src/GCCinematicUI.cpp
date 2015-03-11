/*************************************************************************/
/*!
\file		GCCinematicUI.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCCinematicUI.h"
#include "ComponentTypes.h"
#include "GCEditor.h"

#include "Simian\Game.h"
#include "Simian\CSprite.h"
#include "Simian\Vector3.h"
#include "Simian\CTransform.h"
#include "Simian\EnginePhases.h"
#include "Simian\Interpolation.h"
#include "Simian\Math.h"
#include "Simian\CBitmapText.h"
#include "Simian\LogManager.h"

namespace Descension
{
    static const Simian::f32 BORDER_HEIGHT_RATIO = 0.15f;
    static const Simian::f32 SUBTITLE_HEIGHT_RATIO = 0.3f;
    static const Simian::f32 SUBTITLE_OFFSET = -0.75f * 0.5f;
    static const Simian::f32 SUBTITLE_APPEAR_RATIO = 0.1f;
    static const Simian::f32 SUBTITLE_APPEAR_MAX = 0.25f;
    static const Simian::f32 HD_HEIGHT = 1080.0f;

    GCCinematicUI* GCCinematicUI::instance_ = 0;

    Simian::FactoryPlant<Simian::EntityComponent, GCCinematicUI> GCCinematicUI::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_CINEMATICUI);

    GCCinematicUI::GCCinematicUI()
        : fade_(0),
          fadeTransform_(0),
          cinematicBorderTop_(0),
          cinematicBorderBottom_(0),
          fadeTimer_(0),
          fadeDuration_(0),
          borderHeight_(0),
          borderAmount_(0),
          borderInitial_(0),
          borderFinal_(0),
          borderTimer_(0),
          borderDuration_(0),
          subtitleTimer_(0),
          subtitleDuration_(0),
		  autoFadeOut_(true)
    {
    }

    //--------------------------------------------------------------------------

    GCCinematicUI& GCCinematicUI::Instance()
    {
        return *instance_;
    }


    //--------------------------------------------------------------------------

    void GCCinematicUI::update_( Simian::DelegateParameter& param )
    {
        Simian::EntityUpdateParameter* p;
        param.As(p);

        // make sure border size is the same as the screen
        Simian::Vector2 borderSize = Simian::Vector2(
            Simian::Game::Instance().Size().X(),
            Simian::Game::Instance().Size().Y() * BORDER_HEIGHT_RATIO);
        Simian::Vector2 subtitleBgSize = Simian::Vector2(
            Simian::Game::Instance().Size().Y() * SUBTITLE_HEIGHT_RATIO,
            Simian::Game::Instance().Size().X());
        borderHeight_ = Simian::Game::Instance().Size().Y() * BORDER_HEIGHT_RATIO;
        cinematicBorderTopSprite_->Size(borderSize);
        cinematicBorderBottomSprite_->Size(borderSize);
        subtitleBgSprite_->Size(subtitleBgSize);

        if (fadeTimer_ < fadeDuration_)
        {
            fadeTimer_ += p->Dt;
            Simian::f32 intp = fadeTimer_/fadeDuration_;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            Simian::Color color = Simian::Interpolation::Interpolate(initialColor_, finalColor_, intp, Simian::Interpolation::Linear);
            fade_->Color(color);
        }

        if (borderTimer_ < borderDuration_)
        {
            borderTimer_ += p->Dt;
            Simian::f32 intp = borderTimer_/borderDuration_;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            borderAmount_ = Simian::Interpolation::Interpolate(borderInitial_, borderFinal_, intp, Simian::Interpolation::Linear);
        }

        // calculate position based on border amount
        Simian::f32 hHeight = Simian::Game::Instance().Size().Y() * 0.5f;
        Simian::f32 hBorderHeight = borderHeight_ * 0.5f;
        Simian::f32 pos = hHeight + hBorderHeight - borderAmount_ * borderHeight_;
        Simian::f32 hSubtitleBGHeight = subtitleBgSize.X() * 0.5f;

        cinematicBorderTop_->Position(Simian::Vector3(0.0f, pos, 0.0f));
        cinematicBorderBottom_->Position(Simian::Vector3(0.0f, -pos, 0.0f));
        subtitleBg_->Position(Simian::Vector3(0.0f, -hHeight + hSubtitleBGHeight, 0.0f));
        skipText_->Color(Simian::Color(1.0f, 1.0f, 1.0f, borderAmount_));

        // position the subtitles
        subtitles_->Position(Simian::Vector3(0, SUBTITLE_OFFSET * Simian::Game::Instance().Size().Y(), -0.1f));
        Simian::f32 textScale = Simian::Game::Instance().Size().Y()/HD_HEIGHT;
        subtitles_->Scale(Simian::Vector3(textScale, textScale, textScale));

        if (subtitleTimer_ < subtitleDuration_ || subtitleDuration_ < 0.0f)
        {
            subtitleTimer_ += p->Dt;

            Simian::f32 appearTime = Simian::Math::Min(subtitleDuration_ * SUBTITLE_APPEAR_RATIO, SUBTITLE_APPEAR_MAX);
            Simian::f32 endDiff = subtitleDuration_ - subtitleTimer_;
            Simian::f32 intp = endDiff < appearTime ? endDiff/appearTime : 
                subtitleTimer_ < appearTime ? subtitleTimer_/appearTime : 1.0f;
            intp = subtitleDuration_ < 0.0f ? subtitleTimer_/SUBTITLE_APPEAR_MAX : intp;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

            // animate opacity using intp
            subtitlesText_->Opacity(intp);
            subtitleBgSprite_->Color(Simian::Color(0.0f, 0.0f, 0.0f, Simian::Math::Max(intp - borderAmount_, 0.0f)));
        }
        else
        {
            subtitlesText_->Opacity(0.0f);
            subtitleBgSprite_->Color(Simian::Color(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }

    //--------------------------------------------------------------------------

    void GCCinematicUI::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCCinematicUI, &GCCinematicUI::update_>(this));
    }

    void GCCinematicUI::OnAwake()
    {
        instance_ = this;

        Simian::Entity* entity = ParentEntity()->ChildByName("BlackFade");
        entity->ComponentByType(Simian::C_SPRITE, fade_);
        entity->ComponentByType(Simian::C_TRANSFORM, fadeTransform_);

        borderHeight_ = Simian::Game::Instance().Size().Y() * BORDER_HEIGHT_RATIO;
        Simian::Vector2 cinematicBorderSize(
            Simian::Game::Instance().Size().X(),
            borderHeight_);

        Simian::f32 hscreenheight = Simian::Game::Instance().Size().Y() * 0.5f;
        Simian::f32 hborderheight = cinematicBorderSize.Y() * 0.5f;

        entity = ParentEntity()->ChildByName("CinematicBorderTop"); 
        entity->ComponentByType(Simian::C_TRANSFORM, cinematicBorderTop_);
        entity->ComponentByType(Simian::C_SPRITE, cinematicBorderTopSprite_);
        cinematicBorderTopSprite_->Size(cinematicBorderSize);
        cinematicBorderTop_->Position(Simian::Vector3(0.0f, hscreenheight + hborderheight, 0.0f));

        entity = ParentEntity()->ChildByName("CinematicBorderBottom"); 
        entity->ComponentByType(Simian::C_TRANSFORM, cinematicBorderBottom_);
        entity->ComponentByType(Simian::C_SPRITE, cinematicBorderBottomSprite_);
        cinematicBorderBottomSprite_->Size(cinematicBorderSize);
        cinematicBorderBottom_->Position(Simian::Vector3(0.0f, -hscreenheight - hborderheight, 0.0f));

        entity = ParentEntity()->ChildByName("Subtitles");
        entity->ComponentByType(Simian::C_TRANSFORM, subtitles_);
        entity->ComponentByType(Simian::C_BITMAPTEXT, subtitlesText_);
        subtitles_->Position(Simian::Vector3(0, SUBTITLE_OFFSET * Simian::Game::Instance().Size().Y(), 0));

        entity = ParentEntity()->ChildByName("SubtitleRamp");
        entity->ComponentByType(Simian::C_TRANSFORM, subtitleBg_);
        entity->ComponentByType(Simian::C_SPRITE, subtitleBgSprite_);
        subtitleBgSprite_->Size(Simian::Vector2(cinematicBorderSize.Y(), cinematicBorderSize.X()));
        subtitleBgSprite_->Color(Simian::Color(0.0f, 0.0f, 0.0f, 0.0f));
        subtitleBg_->Position(Simian::Vector3(0.0f, -hscreenheight + hborderheight, 0.0f));

        entity = ParentEntity()->ChildByName("SkipText");
        entity->ComponentByType(Simian::C_SPRITE, skipText_);
        skipText_->Color(Simian::Color(0.0f, 0.0f, 0.0f, 0.0f));

        fade_->Color(Simian::Color(0.0f, 0.0f, 0.0f, 1.0f));

#if _DESCENDED
		if (autoFadeOut_ || GCEditor::EditorMode())
#else
		if (autoFadeOut_)
#endif
			FadeOut(Simian::Color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, -100.0f);
    }

    void GCCinematicUI::FadeOut( const Simian::Color& color, Simian::f32 duration, Simian::f32 depth )
    {
        initialColor_ = fade_->Color();
        finalColor_ = color;
        fadeDuration_ = duration;
        fadeTimer_ = 0;

        Simian::Vector3 fadePos = fadeTransform_->Position();
        fadePos.Z(depth);
        fadeTransform_->Position(fadePos);
    }

    void GCCinematicUI::SkipTextVisible( bool visible )
    {
        if (skipText_)
            skipText_->Visible(visible);
    }

    void GCCinematicUI::ShowCinematicBorders( Simian::f32 duration, bool skipable )
    {
        borderInitial_ = borderAmount_;
        borderFinal_ = 1.0f;
        borderTimer_ = 0;
        borderDuration_ = duration;
        if (skipText_)
            skipText_->Visible(skipable);
    }

    void GCCinematicUI::HideCinematicBorders( Simian::f32 duration )
    {
        borderInitial_ = borderAmount_;
        borderFinal_ = 0.0f;
        borderTimer_ = 0;
        borderDuration_ = duration;
    }

    void GCCinematicUI::ShowSubtitle( const std::string& subtitle, Simian::f32 duration )
    {
        // empty subtitles means we gotta hide the current subtitle.
        if (subtitle == " " || subtitle == "")
        {
            if (subtitleTimer_ < subtitleDuration_)
            {
                subtitleDuration_ = SUBTITLE_APPEAR_MAX/SUBTITLE_APPEAR_RATIO;
                subtitleTimer_ = subtitleDuration_ - SUBTITLE_APPEAR_MAX;
            }
            return;
        }

        subtitleTimer_ = 0.0f;
        subtitleDuration_ = duration;
        subtitlesText_->Opacity(0.0f);
        subtitlesText_->Text(subtitle);
    }

    void GCCinematicUI::Serialize( Simian::FileObjectSubNode& data )
    {
		data.AddData("AutoFadeOut", autoFadeOut_);
    }

    void GCCinematicUI::Deserialize( const Simian::FileObjectSubNode& data )
    {
		data.Data("AutoFadeOut", autoFadeOut_, autoFadeOut_);
    }
}
