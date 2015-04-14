/******************************************************************************/
/*!
\file		AIEffectDisappear.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/11/01
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AIEFFECTDISAPPEAR_H_
#define DESCENSION_AIEFFECTDISAPPEAR_H_

#include "Simian/SimianTypes.h"
#include "Simian/Delegate.h"
#include "Simian/Vector3.h"
#include "Simian/FiniteStateMachine.h"

#include "AIEffectBase.h"

namespace Simian
{
    class CTransform;
    class ParticleSystem;
    class Entity;
    class CModel;
    class CSoundEmitter;
}

namespace Descension
{
    class AIEffectDisappear : public AIEffectBase
    {
    private:
        enum DisappearStates
        {
            DS_PREDISAPPEAR,
            DS_DISAPPEAR,
            DS_APPEAR,
            DS_TOTAL
        };
    private:
        Simian::CTransform* trans_;
        Simian::CTransform* smokeTrans_;
        Simian::Entity* smoke_;
        Simian::ParticleSystem* smokeParticleSys_;
        Simian::CModel* model_;
        Simian::CSoundEmitter* sounds_;
        bool visible_;
        Simian::u32 stateTrigger_;
        Simian::f32 timer_;
        Simian::f32 disappearDelay_;
        Simian::f32 smokeLifeTime_;
        Simian::u32 poofSoundNum_;
        Simian::FiniteStateMachine fsm_;
        static Simian::FactoryPlant<AIEffectBase, AIEffectDisappear> factoryPlant_;
    private:
        void onAwake_();
        void onInit_( GCAttributes* source );
        void onUnload_();
        void serialize_( Simian::FileObjectSubNode& node );
        void deserialize_( const Simian::FileObjectSubNode& node );
        void preDisappearOnEnter_( Simian::DelegateParameter& param );
        void preDisappearOnUpdate_( Simian::DelegateParameter& param );
        void disappearOnEnter_( Simian::DelegateParameter& param );
        void disappearOnUpdate_( Simian::DelegateParameter& param );
        void appearOnEnter_( Simian::DelegateParameter& param );
        void appearOnUpdate_( Simian::DelegateParameter& param );
        void addSmoke_();
    public:
        AIEffectDisappear();
        ~AIEffectDisappear();
        bool Update( Simian::f32 dt);
    };
}

#endif
