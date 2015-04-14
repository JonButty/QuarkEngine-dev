/*************************************************************************/
/*!
\file		GCTrapFireSpot.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCTRAPFIRESPOT_H_
#define DESCENSION_GCTRAPFIRESPOT_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
    class CParticleSystem;
    class CSoundEmitter;
}

namespace Descension
{
    class GCAttributes;

    class GCTrapFireSpot: public Simian::EntityComponent
    {
    private:
        Simian::CTransform *transform_;
        Simian::CTransform *playerTransform_;
        GCAttributes *attributes_, *playerAttributes_;
        Simian::CParticleSystem *fireParticles1_;
        Simian::CSoundEmitter* sounds_;

        Simian::f32 radius_;
        Simian::f32 hitTimer_, hitDelay_;
        Simian::f32 soundTimer_, soundTimerMax_;
        Simian::s32 fireDamage_;
        bool enabled_;

        // factory plant
        static Simian::FactoryPlant<Simian::EntityComponent, GCTrapFireSpot> factoryPlant_;
    public:
        Simian::CTransform& Transform();
        bool Enabled() const;
        void Enabled(bool);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCTrapFireSpot();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
