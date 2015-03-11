/*************************************************************************/
/*!
\file		GCFlamingSnakeEffect.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCFLAMINGSNAKEEFFECT_H_
#define DESCENSION_GCFLAMINGSNAKEEFFECT_H_

#include "Simian/Vector3.h"
#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
    class CParticleSystem;
}

namespace Descension
{
    class GCFlamingSnakeEffect: public Simian::EntityComponent
    {
    private:
        Simian::Vector3 direction_;
        Simian::Vector3 deviationVec_;
        Simian::f32 deviationAmount_;
        Simian::f32 deviation_;
        Simian::f32 duration_;
        Simian::f32 timer_;
        Simian::f32 speed_;
        bool autoDestroy_;
        bool dealDamage_;
        Simian::s32 damage_;

        Simian::CTransform* transform_;
        Simian::CParticleSystem* flames_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCFlamingSnakeEffect> factoryPlant_;
    public:
        const Simian::Vector3& Direction() const;
        void Direction(const Simian::Vector3& val);

        Simian::f32 DeviationAmount() const;
        void DeviationAmount(Simian::f32 val);

        Simian::f32 Duration() const;
        void Duration(Simian::f32 val);

        bool AutoDestroy() const;
        void AutoDestroy(bool val);

        bool DealDamage() const;
        void DealDamage(bool val);

        Simian::s32 Damage() const;
        void Damage(Simian::s32 val);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCFlamingSnakeEffect();
        
        void OnSharedLoad();
        void OnAwake();
        void OnDeinit();

        void Serialize( Simian::FileObjectSubNode& data );
        void Deserialize( const Simian::FileObjectSubNode& data );
    };
}

#endif
