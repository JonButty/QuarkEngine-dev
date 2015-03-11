/******************************************************************************/
/*!
\file		GCAICaster.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCAICASTER_H_
#define DESCENSION_GCAICASTER_H_

#include "Simian/EntityComponent.h"
#include "Simian/Vector3.h"
#include "Simian/Vector2.h"

#include "GCAIBase.h"
#include <vector>

namespace Simian
{
    class CTransform;
    class CModel;
}

namespace Descension
{
    class GCTileMap;
    class GCPhysicsController;
    class GCAttributes;
    class AISkillBase;

    enum EnemyStates
    {
        ES_BIRTH,
        ES_IDLE,
        ES_ATTACK_SKILL,
        ES_ATTACK_DEFAULT,
        ES_DEATH,
        ES_TOTAL
    };

	enum Sounds
    {
        S_ALERT_1,
        S_ALERT_2,
        S_ALERT_3,
        S_DASH,
        S_DIE_1,
        S_DIE_2,
        S_RUN,
        S_SHOOT,
        S_ATTACK,
        S_BIRTH,
        S_MELEE_GET_HIT_1,
        S_MELEE_GET_HIT_2
    };

    class GCAICaster : public GCAIBase
    {
    private:
        GCTileMap* tMap_;
        GCAttributes* playerAttr_;
        Simian::CTransform* playerTransform_;
        GCPhysicsController* playerPhysics_;
		Simian::CSoundEmitter* sounds_;
        bool hasAttacked_;
		bool shadowShown_;
        Simian::u32 attackAnimationsTotal_;
        Simian::u32 animationFrameRate_;
        Simian::u32 chanceValue_;
        Simian::f32 attackRad_;
        Simian::f32 attackTimer_;
        Simian::f32 steerTimer_;
        Simian::f32 attackRadiusSqr_;
        Simian::f32 attackTimeTotal_;
        Simian::f32 attackHitDelay_;
        Simian::f32 bulletStartHeight_;
        Simian::f32 attackDist_;
        Simian::Vector2 oldMin_;
        Simian::Vector2 oldMax_;
        Simian::Vector3 vecToPlayer_;
        Simian::Vector3 nextTile_;
        Simian::Vector3 prevDir_;
        Simian::Vector3 startDir_;
        Simian::Vector3 finalDir_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCAICaster> factoryPlant_;
    public:
        bool HasAttacked() const;
    private:
        void birthOnEnter_( Simian::DelegateParameter& param );
        void birthOnUpdate_( Simian::DelegateParameter& param );
        void idleOnEnter_( Simian::DelegateParameter& param );
        void idleOnUpdate_( Simian::DelegateParameter& param );
        void attackSkillOnEnter_( Simian::DelegateParameter& param );
        void attackSkillOnUpdate_( Simian::DelegateParameter& param );
        void attackDefaultOnEnter_ ( Simian::DelegateParameter& param );
        void attackDefaultOnUpdate_ ( Simian::DelegateParameter& param );
        void deathOnEnter_( Simian::DelegateParameter& param );
        void deathOnUpdate_( Simian::DelegateParameter& param );
        void updateVecToPlayer_();
        void facePlayer_();
        Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);

        void overRideStateUpdate_( Simian::f32 dt );
        // Attack functions
        void helperStartAttack_();
        void createBullet_(Simian::f32 angle);

        void postPhysics_(Simian::DelegateParameter& param);
    public:
        GCAICaster();
        void OnSharedLoad();
        void Deserialize( const Simian::FileObjectSubNode& data );
        void Serialize( Simian::FileObjectSubNode& data );
        void OnAwake();
        void OnInit();
        void OnDeinit();
    };
}

#endif
