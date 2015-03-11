/******************************************************************************/
/*!
\file		GCEnemyProjectileLogic.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCENEMYPROJECTILELOGIC_H_
#define DESCENSION_GCENEMYPROJECTILELOGIC_H_

#include "Simian/EntityComponent.h"
#include "Simian/FiniteStateMachine.h"

#include <string>

namespace Simian
{
    class CTransform;
    class CModelColorOperation;
    class CSoundEmitter;
}

namespace Descension
{
    class GCAttributes;

    class GCEnemyProjectileLogic : public Simian::EntityComponent
    {
    private:
        enum FireBallState
        {
            FBS_BIRTH,
            FBS_FLIGHT,
            FBS_DEATH,
            FBS_TOTAL
        };
    private:
        bool enable_, fadeAfterDestroy_;
        Simian::CTransform* targetTrans_,* currPos_;
        GCAttributes* targetAttrib_;
        Simian::CModelColorOperation* modelColor_;
        Simian::CSoundEmitter* sounds_;
        Simian::s32 damage_;
        Simian::s32 explosionTypeU_;
        Simian::f32 damageRadius_;
        Simian::f32 horizSpeed_, vertInitSpeed_;
        Simian::f32 timer_, flightTime_, explosionTime_;
        Simian::f32 gravity_, halfGravity_;
        Simian::f32 startHeight_;
        std::string explosionType_;
        std::string explosionAnimation_;
        Simian::Vector3 dir_;
        Simian::Vector3 destPos_;
        Simian::FiniteStateMachine fsm_;
        // factory plant
        static Simian::FactoryPlant<Simian::EntityComponent, GCEnemyProjectileLogic> factoryPlant_;
    public:
        // Accessors used to override default values
        bool Enable() const;
        void Enable(bool val);
        void TargetTrans(Simian::CTransform* val);
        Simian::CTransform* TargetTrans();
        GCAttributes* TargetAttrib() const;
        void TargetAttrib(GCAttributes* val);
        Simian::u32 Damage() const;
        void Damage(Simian::u32 val);
        void ExplosionType(const std::string& val);
        Simian::f32 DamageRadius() const;
        void DamageRadius(Simian::f32 val);
        Simian::f32 HorizSpeed() const;
        void HorizSpeed(Simian::f32 val);
        Simian::f32 Gravity() const;
        void Gravity(Simian::f32 val);
    private:
        void update_(Simian::DelegateParameter& param);
        void birthOnEnter_(Simian::DelegateParameter& param);
        void birthOnUpdate_(Simian::DelegateParameter& param);
        void flightOnEnter_(Simian::DelegateParameter& param);
        void flightOnUpdate_(Simian::DelegateParameter& param);
        void deathOnEnter_(Simian::DelegateParameter& param);
        void deathOnUpdate_(Simian::DelegateParameter& param);
    public:
        GCEnemyProjectileLogic();
        void OnSharedLoad();
        void OnAwake();
        void OnInit();
        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);        
    };
}
#endif
