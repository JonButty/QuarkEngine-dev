/*************************************************************************/
/*!
\file		GCLightningSpellEffect.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCLIGHTNINGSPELLEFFECT_H_
#define DESCENSION_GCLIGHTNINGSPELLEFFECT_H_

#include "Simian/Vector3.h"
#include "Simian/EntityComponent.h"
#include "Simian/FiniteStateMachine.h"

namespace Simian
{
    class CTransform;
    class CModelColorOperation;
    class CSoundEmitter;
}

namespace Descension
{
    class GCLightningEffect;
    
    class GCLightningSpellEffect: public Simian::EntityComponent
    {
    private:
        enum LightningSpellState
        {
            LSS_DROP,
            LSS_STRIKE,
            LSS_TOTAL
        };

        struct TrailPair
        {
            Simian::Vector3 Start;
            Simian::Vector3 End;
            Simian::Vector3 Direction;
        };
    private:
        Simian::CTransform* transform_;
        GCLightningEffect* mainBeam_[2];
        GCLightningEffect* childBeams_[5];
        Simian::CModelColorOperation* lightColor_;
        TrailPair trails_[5];
        Simian::CSoundEmitter* sounds_;

        Simian::f32 timer_;
        Simian::f32 flickerTimer_;

        Simian::FiniteStateMachine fsm_;
        bool autoDestroy_;
        
        static Simian::FactoryPlant<Simian::EntityComponent, GCLightningSpellEffect> factoryPlant_;
    private:
        void update_(Simian::DelegateParameter& param);

        void dropEnter_(Simian::DelegateParameter& param);
        void dropUpdate_(Simian::DelegateParameter& param);

        void strikeEnter_(Simian::DelegateParameter& param);
        void strikeUpdate_(Simian::DelegateParameter& param);
    public:
        GCLightningSpellEffect();

        void OnSharedLoad();
        void OnAwake();
        void OnDeinit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
