/*************************************************************************/
/*!
\file		GCParticleChainDeactivator.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/12
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/

#ifndef DESCENSION_GCPARTICLECHAINDEACTIVATOR_H_
#define DESCENSION_GCPARTICLECHAINDEACTIVATOR_H_

#include "Simian/EntityComponent.h"

namespace Descension
{
    class GCParticleChainDeactivator: public Simian::EntityComponent
    {
    private:
        Simian::f32 duration_;
        bool particlesDisabled_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCParticleChainDeactivator> factoryPlant_;
    public:
        Simian::f32 Duration() const;
        void Duration(Simian::f32 val);
    private:
        void update_(Simian::DelegateParameter&);
    public:
        GCParticleChainDeactivator();

        void OnSharedLoad();

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
