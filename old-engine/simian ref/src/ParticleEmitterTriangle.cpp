/*************************************************************************/
/*!
\file		ParticleEmitterTriangle.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "ParticleEmitterTriangle.h"
#include "Simian\Math.h"
#include "Simian\Particle.h"
#include "Simian\ParticleSystem.h"

namespace Descension
{
    ParticleEmitterTriangle::ParticleEmitterTriangle( Simian::ParticleSystem* parent )
        : Simian::ParticleEmitter(parent)
    {
    }

    //--------------------------------------------------------------------------

    const Simian::Vector3* ParticleEmitterTriangle::Vertices() const
    {
        return vertices_;
    }

    void ParticleEmitterTriangle::Vertices( const Simian::Vector3* val )
    {
        vertices_[0] = val[0];
        vertices_[1] = val[1];
        vertices_[2] = val[2];
    }

    //--------------------------------------------------------------------------

    void ParticleEmitterTriangle::Spawn( Simian::Particle& particle )
    {
        Simian::Vector3 uAxis = vertices_[1] - vertices_[0];
        Simian::Vector3 vAxis = vertices_[2] - vertices_[0];

        // average of all points
        Simian::Vector3 center = vertices_[0] + vertices_[1] + vertices_[2];
        center /= 3.0f;

        // randomize a u coordinate
        Simian::f32 u, v;

        if (Simian::Math::Random(0, 2))
        {
            v = Simian::Math::Random(0.0f, 1.0f);
            u = Simian::Math::Random(0.0f, 1.0f - v);
        }
        else
        {
            u = Simian::Math::Random(0.0f, 1.0f);
            v = Simian::Math::Random(0.0f, 1.0f - u);
        }

        // spawn pos
        Simian::Vector3 pos = vertices_[0] + u * uAxis + v * vAxis;

        // set particle spawning position
        particle.Position = pos;

        // put particle into world space
        ParentSystem()->Transform().Transform(particle.Position);
    }
}
