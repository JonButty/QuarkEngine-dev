/************************************************************************/
/*!
\file		Particle.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_PARTICLE_H_
#define SIMIAN_PARTICLE_H_

#include "SimianPlatform.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Color.h"
#include "ParticleSystemFrame.h"

namespace Simian
{
	struct SIMIAN_EXPORT Particle
    {
        u32               ParticleId;
        f32               Time;
        f32               LifeTime;
		Simian::Vector3   Position; // always the world position
        Simian::Vector3   Origin;
		Simian::Vector3   Velocity;
        Simian::Vector3   WorldVelocity;
		Simian::Vector3   Scale;
        Simian::Vector3   Rotation;
		Simian::Vector3   AngularVelocity;
		Simian::Color	  Color;

        // current and next snapshot for interpolation
        u32                    FrameIndex;
        ParticleSystemSnapshot Current;
        ParticleSystemSnapshot Next;
	};
}

#endif
