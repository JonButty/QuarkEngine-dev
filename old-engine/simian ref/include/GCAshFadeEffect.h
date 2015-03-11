/*****************************************************************************/
/*!
\file		GCAshFadeEffect.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef DESCENSION_GCASHFADEEFFECT_H_
#define DESCENSION_GCASHFADEEFFECT_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
    class CParticleSystem;
}

namespace Descension
{
    class GCAshFadeEffect: public Simian::EntityComponent
    {
    private:
        Simian::CParticleSystem* particles_;
        bool enabled_;
        Simian::f32 destroyTimer_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCAshFadeEffect> factoryPlant_;
    public:
        bool Enabled() const;
        void Enabled(bool val);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCAshFadeEffect();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();
        void OnDeinit();

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
