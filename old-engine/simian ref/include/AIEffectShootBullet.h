/******************************************************************************/
/*!
\file		AIEffectShootBullet.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/11/01
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AIEFFECTSHOOTBULLET_H_
#define DESCENSION_AIEFFECTSHOOTBULLET_H_

#include "Simian\FiniteStateMachine.h"
#include "Simian\Vector3.h"

#include "AIEffectBase.h"

namespace Simian
{
    class CTransform;
    class CModel;
    class CSoundEmitter;
}

namespace Descension
{
    class GCAttributes;

    class AIEffectShootBullet : public AIEffectBase
    {
    private:
        enum ShootStates
        {
            SS_CHARGE,
            SS_SHOOT,
            SS_TOTAL
        };
    private: 
        Simian::CSoundEmitter* sounds_;
        Simian::CTransform* ownerTrans_;
        Simian::CTransform* targetTrans_;
        Simian::CModel* model_;
        GCAttributes* targetAttribs_;
        bool chanceHit_;
        Simian::u32 stateTrigger_;
        Simian::f32 attackRadius_,attackRadiusSqr_;
        Simian::f32 chargeTime_, shootTime_, tryAgainTime_;
        Simian::f32 timer_,tryAgainTimer_;
        Simian::f32 chance_;
        Simian::f32 bulletStartHeight_;
        Simian::f32 shootAngle_;
        Simian::Vector3 vecToPlayer_;
        Simian::FiniteStateMachine fsm_;

        static Simian::FactoryPlant<AIEffectBase, AIEffectShootBullet> factoryPlant_;
    private:
        void onAwake_();
        void onInit_( GCAttributes* source );
        void onUnload_();
        void serialize_( Simian::FileObjectSubNode& node );
        void deserialize_( const Simian::FileObjectSubNode& node );
        void chargeOnEnter_( Simian::DelegateParameter& param );
        void chargeOnUpdate_( Simian::DelegateParameter& param );
        void shootOnEnter_( Simian::DelegateParameter& param );
        void shootOnUpdate_( Simian::DelegateParameter& param );
        Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);
    public:
        AIEffectShootBullet();
        ~AIEffectShootBullet();
        bool Update( Simian::f32 dt );
    };
}

#endif
