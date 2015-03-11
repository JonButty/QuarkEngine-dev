/************************************************************************/
/*!
\file		ParticleEmitterDisc.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_PARTICLEEMITTERDISC_H_
#define SIMIAN_PARTICLEEMITTERDISC_H_

#include "SimianPlatform.h"
#include "ParticleEmitter.h"

#include "Simian/Vector3.h"

namespace Simian
{
    class SIMIAN_EXPORT ParticleEmitterDisc: public ParticleEmitter
    {
    private:
        float radius_;
        float randomRadius_;
        Vector3 normal_;
        Vector3 forward_;
    public:
        float Radius() const;
        void Radius(float val);

        float RandomRadius() const;
        void RandomRadius(float val);

        const Simian::Vector3& Normal() const;
        void Normal(const Simian::Vector3& val);

        const Simian::Vector3& Forward() const;
        void Forward(const Simian::Vector3& val);
    public:
        ParticleEmitterDisc(ParticleSystem* parent);

        void Spawn( Particle& p );
    };
}

#endif
