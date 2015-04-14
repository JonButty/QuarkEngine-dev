/******************************************************************************/
/*!
\file		GCAICrystalBossFist.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/12/07
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCAICRYSTALBOSSFIST_H_
#define DESCENSION_GCAICRYSTALBOSSFIST_H_

#include "GCAIBase.h"

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCPhysicsController;
    class GCAttributes;

    class GCAICrystalBossFist : public GCAIBase
    {
    private:
        enum State
        {
            CGF_UPDATE,
            CGF_TOTAL
        };
    private: 
        bool attackOn_;
        Simian::CTransform* playerTrans_;
        GCAttributes* bossAttribs_;
        Simian::s32 damage_;
        Simian::f32 radius_;
        Simian::f32 sqrDist_;
        static Simian::FactoryPlant<Simian::EntityComponent, GCAICrystalBossFist> factoryPlant_;
    public:
        void AttackOn(bool val);
        void Damage(Simian::s32 damage);
        void Radius(Simian::f32 val);
    private:
        void update_(Simian::DelegateParameter& param);
        void overRideStateUpdate_( Simian::f32 dt );
        void onTakeDamage_(Simian::DelegateParameter& param);
        void dealDamage_();
    public:
        GCAICrystalBossFist();
        ~GCAICrystalBossFist();
        void OnSharedLoad();
        void OnInit();
        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
        bool IsPlayerInRange();
        Simian::Vector3 Position() const;
    };
}

#endif
