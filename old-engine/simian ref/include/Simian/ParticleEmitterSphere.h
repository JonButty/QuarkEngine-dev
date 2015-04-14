/************************************************************************/
/*!
\file		ParticleEmitterSphere.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_PARTICLEEMITTERCIRCLE_H_
#define SIMIAN_PARTICLEEMITTERCIRCLE_H_

#include "SimianPlatform.h"
#include "ParticleEmitter.h"

namespace Simian
{
    class SIMIAN_EXPORT ParticleEmitterSphere: public ParticleEmitter
    {
    private:
        f32 radius_;
        f32 randomRadius_;
    public:
        Simian::f32 Radius() const;
        void Radius(Simian::f32 val);

        Simian::f32 RandomRadius() const;
        void RandomRadius(Simian::f32 val);
    public:
        ParticleEmitterSphere(ParticleSystem* parent);

        virtual void Spawn( Particle& particle );
    };
}

#endif
