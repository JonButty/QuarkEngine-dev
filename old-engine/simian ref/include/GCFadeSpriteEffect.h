/******************************************************************************/
/*!
\file		GCFadeSpriteEffect.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCFADESPRITE_H_
#define DESCENSION_GCFADESPRITE_H_

#include "Simian/EntityComponent.h"
#include "Simian/FiniteStateMachine.h"

namespace Simian
{
    class CSprite;
}

namespace Descension
{
    class GCFadeSprite : public Simian::EntityComponent
    {
    private:
        enum FadeState
        {
            FS_IDLE,
            FS_FADE,
            FS_TOTAL
        };
    private:
        bool enable_,autoStart_, start_, fadeIn_;
        Simian::CSprite* sprite_;
        Simian::f32 idleTime_,fadeTime_;
        Simian::f32 timer_;
        Simian::FiniteStateMachine fsm_;
        static Simian::FactoryPlant<Simian::EntityComponent, GCFadeSprite> factoryPlant_;
    public:
        bool AutoStart() const;
        void AutoStart(bool val);
        bool Start() const;
        void Start(bool val);
        bool FadeIn() const;
        void FadeIn(bool val);
        void ForceEnd();
        Simian::f32 IdleTime() const;
        void IdleTime(Simian::f32 val);
        Simian::f32 FadeTime() const;
        void FadeTime(Simian::f32 val);
    private:
        void update_(Simian::DelegateParameter& param);
        void idleOnEnter_( Simian::DelegateParameter& param);
        void idleOnUpdate_( Simian::DelegateParameter& param);
        void fadeOnEnter_( Simian::DelegateParameter& param);
        void fadeOnUpdate_( Simian::DelegateParameter& param);

    public:
        GCFadeSprite();
        ~GCFadeSprite();

        void Reset();
        void OnSharedLoad();
        void OnAwake();
        void OnInit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
