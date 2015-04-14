/******************************************************************************/
/*!
\file		AISkillFireBall.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/22
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/

#ifndef DESCENSION_AISKILLFIREBALL_H_
#define DESCENSION_AISKILLFIREBALL_H_

#include "Simian/Delegate.h"
#include "Simian/FiniteStateMachine.h"
#include "Simian/Vector3.h"

#include "AISkillBase.h"

namespace Simian
{
    class CTransform;
    class CModel;
    class Entity;
}

namespace Descension
{
    class GCAttributes;
    class GCEnemyFireBallLogic;

    class AISkillFireBall : public AISkillBase
    {
    private:
        enum FireBallState
        {
            FB_START,
            FB_CHARGE,
            FB_ATTACK,
            FB_END,
            FB_TOTAL
        };
    private:
        bool alive_;
        Simian::CTransform* targetTrans_;
        Simian::CTransform* trans_;
        Simian::CTransform* fireBallTrans_;
        Simian::CTransform* destinationTrans_;
        Simian::CModel* model_;
        Simian::Entity* fireBall_,* fireBallExplosion_;
        GCEnemyFireBallLogic* fireBallLogic_;
        Simian::u32 damage_;
        Simian::f32 damageRadius_;
        Simian::f32 horizSpeed_;
        Simian::f32 gravity_;
        Simian::f32 timer_;
        Simian::f32 chargeTime_, explosionTime_;
        Simian::f32 attackRadius_, attackRadiusSqr_;
        Simian::f32 direction_;
        Simian::f32 bulletStartHeight_;
        Simian::Vector3 vecToPlayerCurr_, vecToPlayerInst_;
        Simian::FiniteStateMachine fsm_;
        static Simian::FactoryPlant<AIEffectBase, AISkillFireBall> factoryPlant_;
    private:
        void onAwake_();
        void onInit_( GCAttributes* source );
        void onUnload_();
        void serialize_( Simian::FileObjectSubNode& node );
        void deserialize_( const Simian::FileObjectSubNode& node );
        void startOnEnter_( Simian::DelegateParameter& param );
        void startOnUpdate_( Simian::DelegateParameter& param );
        void chargeOnEnter_( Simian::DelegateParameter& param );
        void chargeOnUpdate_( Simian::DelegateParameter& param );
        void attackOnEnter_( Simian::DelegateParameter& param );
        void attackOnUpdate_( Simian::DelegateParameter& param );    
        void endOnEnter_( Simian::DelegateParameter& param );
        void endOnUpdate_( Simian::DelegateParameter& param );
        void facePlayer_();
        Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);
        void updateVecToPlayer_();
    public:
        AISkillFireBall();
        ~AISkillFireBall();
        bool Update( Simian::f32 dt );
    };
}
#endif
