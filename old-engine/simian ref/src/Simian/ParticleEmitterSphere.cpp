/************************************************************************/
/*!
\file		ParticleEmitterSphere.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/ParticleEmitterSphere.h"
#include "Simian/Math.h"
#include "Simian/Particle.h"
#include "Simian/ParticleSystem.h"

#include <cmath>

namespace Simian
{
    ParticleEmitterSphere::ParticleEmitterSphere(ParticleSystem* parent)
        : ParticleEmitter(parent), 
          radius_(0.0f),
          randomRadius_(0.0f)
    {
    }

    //--------------------------------------------------------------------------

    void ParticleEmitterSphere::Radius(Simian::f32 val)
    {
        radius_ = val;
    }

    f32 ParticleEmitterSphere::Radius() const
    {
        return radius_;
    }

    Simian::f32 ParticleEmitterSphere::RandomRadius() const
    {
        return randomRadius_;
    }

    void ParticleEmitterSphere::RandomRadius( Simian::f32 val )
    {
        randomRadius_ = val;
    }

    //--------------------------------------------------------------------------

    void ParticleEmitterSphere::Spawn( Particle& particle )
    {
        // randomize an angle for yaw
        f32 yaw = Math::Random(0.0f, Math::TAU);
        f32 pitch = Math::Random(0.0f, Math::TAU);

        // randomize length
        f32 length = radius_ + Math::Random(0.0f, randomRadius_);

        f32 cosYaw = std::cos(yaw);
        f32 sinYaw = std::sin(yaw);
        f32 cosPitch = std::cos(pitch);
        f32 sinPitch = std::sin(pitch);

        // compute outward vector
        Vector3 out(cosPitch * sinYaw, -sinPitch, cosPitch * cosYaw);

        // compute initial position
        particle.Position = out * length;

        ParentSystem()->Transform().Transform(particle.Position);
    }
}
