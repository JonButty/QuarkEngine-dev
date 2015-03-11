/*************************************************************************/
/*!
\file		GCFlameEffect.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCFLAMEEFFECT_H_
#define DESCENSION_GCFLAMEEFFECT_H_

#include "Simian/Vector3.h"
#include "Simian/EntityComponent.h"

namespace Simian
{
    class CParticleSystem;
}

namespace Descension
{
    class GCParticleEmitterTriangle;

    class GCFlameEffect: public Simian::EntityComponent
    {
    private:
        Simian::CParticleSystem* flames_;
        Simian::CParticleSystem* lights_;

        GCParticleEmitterTriangle* flamesEmitter_;
        GCParticleEmitterTriangle* lightsEmitter_;

        Simian::Vector3 vertices_[3];

        bool autoDestroy_;
        Simian::f32 timer_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCFlameEffect> factoryPlant_;
    public:
        void Vertices(const Simian::Vector3* vertices);

        bool AutoDestroy() const;
        void AutoDestroy(bool val);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCFlameEffect();

        void OnSharedLoad();
        void OnInit();

        void Serialize( Simian::FileObjectSubNode& data );
        void Deserialize( const Simian::FileObjectSubNode& data );
    };
}

#endif
