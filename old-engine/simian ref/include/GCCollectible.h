/*************************************************************************/
/*!
\file		GCCollectible.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCCOLLECTIBLE_H_
#define DESCENSION_GCCOLLECTIBLE_H_

#include "Simian/EntityComponent.h"
#include "Simian/Vector3.h"
#include "Simian/FiniteStateMachine.h"

namespace Simian
{
    class CTransform;
    class CSoundEmitter;
};

namespace Descension
{
    class GCCollectible: public Simian::EntityComponent
    {
    private:
        enum CollectibleState
        {
            CS_DROPPING,
            CS_COLLECTING,
            CS_DYING,
            CS_TOTAL
        };
    private:
        Simian::Vector3 velocity_;

        Simian::u32 healthIncrement_;
        Simian::f32 manaIncrement_;
        Simian::f32 yawVelocity_;
        Simian::f32 pitchVelocity_;
        Simian::f32 spinVelocity_;
        Simian::f32 yaw_;
        Simian::f32 pitch_;
        Simian::f32 spin_;
        Simian::f32 timer_;
        Simian::f32 collectionRadius_;
        Simian::f32 collectionTime_;
        Simian::f32 deathDelay_;
        Simian::f32 minDelay_;
        Simian::f32 groundOffset_;
        bool gravity_;

        Simian::FiniteStateMachine fsm_;

        Simian::CTransform* transform_;
        Simian::CSoundEmitter* sounds_;

        static Simian::FactoryPlant<EntityComponent, GCCollectible> factoryPlant_;
    public:
        const Simian::Vector3& Velocity() const;
        void Velocity(const Simian::Vector3& val);
    private:
        void onUpdate_(Simian::DelegateParameter& param);

        void droppingUpdate_(Simian::DelegateParameter& param);
        void collectingUpdate_(Simian::DelegateParameter& param);
        void dyingUpdate_(Simian::DelegateParameter& param);
    public:
        GCCollectible();

        void OnSharedLoad();
        void OnAwake();
        void OnDeinit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
