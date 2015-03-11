/************************************************************************/
/*!
\file		ParticleEmitter.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_PARTICLEEMITTER_H_
#define SIMIAN_PARTICLEEMITTER_H_

#include "SimianPlatform.h"

namespace Simian
{
	class ParticleSystem;
    struct Particle;

	class SIMIAN_EXPORT ParticleEmitter
	{
    private:
		ParticleSystem* parentSystem_;
	public:
		ParticleSystem* ParentSystem() const { return parentSystem_; }
	public:
		ParticleEmitter(ParticleSystem* parent)
			: parentSystem_(parent) {}
        ~ParticleEmitter() {}

        virtual void Spawn(Particle&) {}
	};
}

#endif
