/************************************************************************/
/*!
\file		ParticleEmitterDisc.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/ParticleEmitterDisc.h"
#include "Simian/Math.h"
#include "Simian/Particle.h"
#include "Simian/ParticleSystem.h"

#include <cmath>

namespace Simian
{
    ParticleEmitterDisc::ParticleEmitterDisc( ParticleSystem* parent )
        : ParticleEmitter(parent),
          normal_(Simian::Vector3::UnitY),
          forward_(Simian::Vector3::UnitZ)
    {
    }

    //--------------------------------------------------------------------------

    float ParticleEmitterDisc::Radius() const
    {
        return radius_;
    }

    void ParticleEmitterDisc::Radius( float val )
    {
        radius_ = val;
    }

    float ParticleEmitterDisc::RandomRadius() const
    {
        return randomRadius_;
    }

    void ParticleEmitterDisc::RandomRadius( float val )
    {
        randomRadius_ = val;
    }

    const Simian::Vector3& ParticleEmitterDisc::Normal() const
    {
        return normal_;
    }

    void ParticleEmitterDisc::Normal( const Simian::Vector3& val )
    {
        normal_ = val;
    }

    const Simian::Vector3& ParticleEmitterDisc::Forward() const
    {
        return forward_;
    }

    void ParticleEmitterDisc::Forward( const Simian::Vector3& val )
    {
        forward_ = val;
    }

    //--------------------------------------------------------------------------

    void ParticleEmitterDisc::Spawn( Particle& particle )
    {
        // create the 2 axis for rotation
        Simian::Vector3 xAxis = normal_.Cross(forward_);
        xAxis.Normalize();
        Simian::Vector3 yAxis = xAxis.Cross(normal_);

        // randomize an angle for yaw
        f32 angle = Math::Random(0.0f, Math::TAU);

        // randomize length
        f32 length = radius_ + Math::Random(0.0f, randomRadius_);

        f32 cosA = std::cos(angle);
        f32 sinA = std::sin(angle);

        // compute outward vector
        Vector3 out(xAxis * cosA + yAxis * sinA);

        // compute initial position
        particle.Position = out * length;

        ParentSystem()->Transform().Transform(particle.Position);
    }
}
