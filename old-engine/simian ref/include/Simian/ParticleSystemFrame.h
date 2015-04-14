/************************************************************************/
/*!
\file		ParticleSystemFrame.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_PARTICLESYSTEMFRAME_H_
#define SIMIAN_PARTICLESYSTEMFRAME_H_

#include "SimianPlatform.h"
#include "Vector3.h"
#include "Color.h"

namespace Simian
{
    struct SIMIAN_EXPORT ParticleSystemSnapshot
    {
        f32           Time;
        Vector3       Velocity;
        Vector3       TangentVelocity;
        Vector3       Scale;
        Vector3       Rotation;
        Vector3       AngularVelocity;
        Simian::Color Color;
    };
    
	struct SIMIAN_EXPORT ParticleSystemFrame
	{
        f32           Time;
        u32           Caps;
		Vector3       Velocity;
		Vector3       RandomVelocity;
		Vector3       TangentVelocity;
		Vector3       RandomTangentVelocity;
		Vector3       Scale;
        Vector3       RandomScale;
        Vector3       Rotation;
        Vector3       RandomRotation;
		Vector3       AngularVelocity;
		Vector3       RandomAngularVelocity;
		Simian::Color Color;
		Simian::Color RandomColor;

        ParticleSystemFrame()
            : Time(0.0f),
              Caps(0xFFFF),
              Scale(1.0f, 1.0f, 1.0f),
              RandomColor(0.0f, 0.0f, 0.0f, 0.0f) {}
	};
}

#endif
