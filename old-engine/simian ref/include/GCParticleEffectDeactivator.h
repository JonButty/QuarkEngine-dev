/*************************************************************************/
/*!
\file		GCParticleEffectDeactivator.h
\author 	Gavin Yeo, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/09/29
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCPARTICLEEFFECTDEACTIVATOR_H_
#define DESCENSION_GCPARTICLEEFFECTDEACTIVATOR_H_

#include "Simian/EntityComponent.h"
#include "Simian/Vector3.h"

namespace Simian
{
    class CParticleSystem;
};

namespace Descension
{
    class GCParticleEffectDeactivator: public Simian::EntityComponent
    {
    private:
        Simian::CParticleSystem* particles_;
        Simian::f32 maxTimer_, timer_;
        
        bool queueForRemoval_;

        static Simian::FactoryPlant<EntityComponent, GCParticleEffectDeactivator> factoryPlant_;
    public:
        Simian::f32 MaxTimer() const;
        void MaxTimer(Simian::f32 val);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCParticleEffectDeactivator();

        void OnSharedLoad();
        void OnAwake();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
