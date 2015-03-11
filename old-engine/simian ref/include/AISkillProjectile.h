/******************************************************************************/
/*!
\file		AISkillProjectile.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/22
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AISKILLPROJECTILE_H_
#define DESCENSION_AISKILLPROJECTILE_H_

#include "Simian/Delegate.h"
#include "Simian/FiniteStateMachine.h"
#include "Simian/Vector3.h"

#include "AISkillBase.h"

#include <string>

namespace Simian
{
    class CTransform;
    class CModel;
    class Entity;
}

namespace Descension
{
    class GCAttributes;
    class GCEnemyProjectileLogic;

    class AISkillProjectile : public AISkillBase
    {
    private:
        enum FireBallState
        {
            FB_START,
            FB_CHARGE,
            FB_ATTACK,
            FB_COOLDOWN,
            FB_TOTAL
        };
    private:
        bool alive_;
        Simian::CTransform* targetTrans_;
        Simian::CTransform* trans_;
        Simian::CTransform* projectileTrans_;
        Simian::CTransform* destinationTrans_;
        Simian::CModel* model_;
        Simian::Entity* projectile_,* fireBallExplosion_;
        GCEnemyProjectileLogic* projectileLogic_;
        Simian::u32 damage_;
        Simian::s32 projectileTypeU_;
        Simian::f32 damageRadius_;
        Simian::f32 horizSpeed_;
        Simian::f32 gravity_;
        Simian::f32 timer_;
        Simian::f32 chargeTime_, attackTime_, cooldownTime_;
        Simian::f32 attackRadius_, attackRadiusSqr_;
        Simian::f32 direction_;
        Simian::f32 startHeight_;
        std::string projectileType_;
        std::string explosionType_;
        Simian::Vector3 vecToPlayerCurr_, vecToPlayerInst_;
        Simian::FiniteStateMachine fsm_;
        
        static Simian::FactoryPlant<AIEffectBase, AISkillProjectile> factoryPlant_;
    private:
        void onAwake_();
        void onInit_( GCAttributes* source );
        void onUnload_();
        void serialize_( Simian::FileObjectSubNode& node );
        void deserialize_( const Simian::FileObjectSubNode& node );
        void startOnEnter_( Simian::DelegateParameter& param );
        void startOnUpdate_( Simian::DelegateParameter& param );
        void chargeOnEnter_( Simian::DelegateParameter& param );
        void attackOnEnter_( Simian::DelegateParameter& param );
        void cooldownOnEnter_( Simian::DelegateParameter& param );
        void cooldownOnUpdate_( Simian::DelegateParameter& param );
        void facePlayer_();
        Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);
        void updateVecToPlayer_();
        
        // generator function for casting update callback
        template <Simian::u32 NextState>
        void castingUpdate_(Simian::DelegateParameter& p)
        {
            timer_ -= p.As<Simian::EntityUpdateParameter>().Dt;
            if(timer_ < 0)
                fsm_.ChangeState(NextState);
        }
    public:
        AISkillProjectile();
        ~AISkillProjectile();
        bool Update( Simian::f32 dt );
    };
}
#endif
