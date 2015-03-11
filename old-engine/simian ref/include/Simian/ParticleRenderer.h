/************************************************************************/
/*!
\file		ParticleRenderer.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_PARTICLERENDERER_H_
#define SIMIAN_PARTICLERENDERER_H_

#include "SimianPlatform.h"
#include "Matrix.h"

namespace Simian
{
	class ParticleSystem;
    class RenderQueue;
    struct Particle;

	class SIMIAN_EXPORT ParticleRenderer
	{
	private:
		ParticleSystem* parentSystem_;
	public:
		ParticleSystem* ParentSystem() const { return parentSystem_; }
	public:
		ParticleRenderer(ParticleSystem* system)
			: parentSystem_(system) {}
        virtual ~ParticleRenderer() {}

        virtual void Load() {}
        virtual void Unload() {}

		virtual void Update(f32) {}
		virtual void Render() {}
        virtual void Render(RenderQueue&, u8, const Simian::Matrix&) {}

        virtual void OnSpawn(Particle&) {}
	};
}

#endif
