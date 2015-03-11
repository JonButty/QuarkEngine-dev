/*************************************************************************/
/*!
\file		GCCinematicUI.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_CINEMATICUI_H_
#define DESCENSION_CINEMATICUI_H_

#include "Simian/EntityComponent.h"
#include "Simian/Color.h"

namespace Simian
{
    class CSprite;
    class CTransform;
    class CBitmapText;
}

namespace Descension
{
    class GCCinematicUI: public Simian::EntityComponent
    {
    private:
        Simian::CSprite* fade_;
        Simian::CTransform* fadeTransform_;
        Simian::CTransform* cinematicBorderTop_;
        Simian::CSprite* cinematicBorderTopSprite_;
        Simian::CTransform* cinematicBorderBottom_;
        Simian::CSprite* cinematicBorderBottomSprite_;
        Simian::CTransform* subtitles_;
        Simian::CBitmapText* subtitlesText_;
        Simian::CTransform* subtitleBg_;
        Simian::CSprite* subtitleBgSprite_;
        Simian::CSprite* skipText_;

        static GCCinematicUI* instance_;

        Simian::Color initialColor_;
        Simian::Color finalColor_;
        Simian::f32 fadeTimer_;
        Simian::f32 fadeDuration_;

        Simian::f32 borderHeight_;
        Simian::f32 borderAmount_;
        Simian::f32 borderInitial_;
        Simian::f32 borderFinal_;
        Simian::f32 borderTimer_;
        Simian::f32 borderDuration_;
        Simian::f32 subtitleTimer_;
        Simian::f32 subtitleDuration_;

		bool autoFadeOut_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCCinematicUI> factoryPlant_;
    public:
        static GCCinematicUI& Instance();
    private:
        void update_(Simian::DelegateParameter&);
    public:
        GCCinematicUI();

        void OnSharedLoad();
        void OnAwake();

        void FadeOut(const Simian::Color& color, Simian::f32 duration, Simian::f32 depth = 0.0f);

        void SkipTextVisible(bool visible);
        void ShowCinematicBorders(Simian::f32 duration, bool skipable = true);
        void HideCinematicBorders(Simian::f32 duration);

        void ShowSubtitle(const std::string& subtitle, Simian::f32 duration);

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
