/******************************************************************************/
/*!
\file		AIEffectPlayAnim.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/11/01
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AIEFFECTPLAYANIM_H_
#define DESCENSION_AIEFFECTPLAYANIM_H_

#include "Simian\FiniteStateMachine.h"

#include "AIEffectBase.h"

#include <string>

namespace Simian
{
    class CModel;
    class CSoundEmitter;
}

namespace Descension
{
    class AIEffectPlayAnim : public AIEffectBase
    {
    private:
        enum PlayStates
        {
            PS_IDLE,
            PS_PLAY,
            PS_TOTAL
        };
    private:
        Simian::CModel* animationModel_;
        Simian::CSoundEmitter* soundEmitter_;
        bool chanceHit_;
        bool soundPlayed_;
        Simian::u32 fps_;
        Simian::u32 stateTrigger_;
        Simian::f32 playChance_;
        Simian::f32 movementSpeed_;
        Simian::f32 playTime_;
        Simian::f32 timer_, tryAgainTimer_;
        std::string animationName_;
        std::string soundName_;
        Simian::u32 soundNum_;
        Simian::FiniteStateMachine fsm_;

        static Simian::FactoryPlant<AIEffectBase, AIEffectPlayAnim> factoryPlant_;
    private:
        void onAwake_();
        void onInit_( GCAttributes* source );
        void onUnload_();
        void serialize_( Simian::FileObjectSubNode& node );
        void deserialize_( const Simian::FileObjectSubNode& node );
        void idleOnEnter_( Simian::DelegateParameter& param );
        void idleOnUpdate_( Simian::DelegateParameter& param );
        void playOnEnter_( Simian::DelegateParameter& param );
        void playOnUpdate_( Simian::DelegateParameter& param );
    public:
        AIEffectPlayAnim();
        ~AIEffectPlayAnim();
        bool Update( Simian::f32 dt );
    };
}
#endif
