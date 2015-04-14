/******************************************************************************/
/*!
\file		AISkillGroundSmash.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AISKILLGROUNDSMASH_H_
#define DESCENSION_AISKILLGROUNDSMASH_H_

#include "Simian\FiniteStateMachine.h"

#include "AISkillBase.h"

namespace Simian
{
    class CModel;
    class CTransform;
    class CSoundEmitter;
}

namespace Descension
{
    class GCAttributes;
    class GCBoneTrailVelocityController;

    class AISkillGroundSmash : public AISkillBase
    {
    private:
        enum SmashStates
        {
            SS_BIRTH,
            SS_CHARGE,
            SS_SMASH,
            SS_COOLDOWN,
            SS_TOTAL
        };
    private:
        GCAttributes* targetAttribs_;
        Simian::CModel* model_;
        Simian::CTransform* ownersTrans_;
        Simian::CTransform* srcTrans_;
        GCBoneTrailVelocityController* boneTrail1_;
        GCBoneTrailVelocityController* boneTrail2_;
        Simian::CSoundEmitter* sounds_;
        bool smash_;
        Simian::s32 damage_;
        Simian::u32 slamSoundNum_;
        Simian::f32 attackRadius_, attackRadiusSqr_;
        Simian::f32 damageRadius_, damageRadiusSqr_;
        Simian::f32 chargeTime_, smashTime_, coolDownTime_;
        Simian::f32 timer_;
        Simian::Vector3 vecToTar_;
        Simian::FiniteStateMachine fsm_;

        static Simian::FactoryPlant<AIEffectBase, AISkillGroundSmash> factoryPlant_;
    private:
        void onAwake_();
        void onInit_( GCAttributes* source );
        void onUnload_();
        void serialize_( Simian::FileObjectSubNode& node );
        void deserialize_( const Simian::FileObjectSubNode& node );
        void birthOnEnter_( Simian::DelegateParameter& param );
        void birthOnUpdate_( Simian::DelegateParameter& param );
        void chargeOnEnter_( Simian::DelegateParameter& param );
        void chargeOnUpdate_( Simian::DelegateParameter& param );
        void smashOnEnter_( Simian::DelegateParameter& param );
        void smashOnUpdate_( Simian::DelegateParameter& param );
        void coolDownOnEnter_( Simian::DelegateParameter& param );
        void coolDownOnUpdate_( Simian::DelegateParameter& param );
        void updateVecToTarget_();
        Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);
    public:
        AISkillGroundSmash();
        ~AISkillGroundSmash();
        bool Update( Simian::f32 dt );
    };
}

#endif
