/************************************************************************/
/*!
\file		ParticleSystem.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_PARTICLESYSTEM_H_
#define SIMIAN_PARTICLESYSTEM_H_

#include "SimianPlatform.h"
#include "Particle.h"
#include "ParticleSystemFrame.h"
#include "Matrix.h"

#include <vector>

namespace Simian
{
	class ParticleRenderer;
	class ParticleEmitter;
	class RenderQueue;

	class SIMIAN_EXPORT ParticleSystem
	{
    public:
        enum FrameCaps
        {
            FC_VELOCITY                 = 0x0001,
            FC_RANDOMVELOCITY           = 0x0002,
            FC_TANGENTVELOCITY          = 0x0004,
            FC_RANDOMTANGENTVELOCITY    = 0x0008,
            FC_SCALE                    = 0x0010,
            FC_RANDOMSCALE              = 0x0020,
            FC_ANGULARVELOCITY          = 0x0040,
            FC_RANDOMANGULARVELOCITY    = 0x0080,
            FC_COLOR                    = 0x0100,
            FC_RANDOMCOLOR              = 0x0200,
            FC_ROTATION                 = 0x0400,
            FC_RANDOMROTATION           = 0x0800,
            FC_ALL                      = 0xFFFF
        };
	private:
		std::vector<Particle> particles_;
        std::vector<Particle*> freeParticles_;
        std::vector<Particle*> activeParticles_;
        std::vector<ParticleSystemFrame> frames_;
        ParticleRenderer* renderer_;
		ParticleEmitter* emitter_;

        // spawning variables
        f32 spawnInterval_;
        f32 randomSpawnInterval_;

        u32 spawnAmount_;
        u32 randomSpawnAmount_;

        f32 lifeTime_;
        f32 randomLifeTime_;

        // world transform
        Matrix transform_;

        // timer for spawning.
        f32 spawnTimer_;

        bool enabled_;
    public:
        u32 TotalParticles() const;

        void SpawnInterval(f32 val);
        f32 SpawnInterval() const;

        void RandomSpawnInterval(f32 val);
        f32 RandomSpawnInterval() const;

        void SpawnAmount(u32 val);
        u32 SpawnAmount() const;

        void RandomSpawnAmount(u32 val);
        u32 RandomSpawnAmount() const;

        void LifeTime(f32 val);
        f32 LifeTime() const;

        void RandomLifeTime(f32 val);
        f32 RandomLifeTime() const;

        const Matrix& Transform() const;
        void Transform(const Matrix& val);

        bool Enabled() const;
        void Enabled(bool val);

        const std::vector<Particle>& Particles() const;
        const std::vector<Particle*>& ActiveParticles() const;
        const std::vector<ParticleSystemFrame>& Frames() const;
    public:
        void initParticle_(Particle& particle);
	public:
		ParticleSystem();

        void Load(u32 totalParticles);
        void Unload();

		template <class T>
		T* AddRenderer()
		{
            if (renderer_)
                renderer_->Unload();
			delete renderer_;
            T* ret = new T(this);
            renderer_ = ret;
			return ret;
		}

		template <class T>
		T* AddEmitter()
		{
			delete emitter_;
            T* ret = new T(this);
            emitter_ = ret;
			return ret;
		}

		/** Adds a particle system frame.
			@remarks
				This causes a reset of all particles.
		*/
		void AddFrame(const ParticleSystemFrame& frame);
		/** Clears all particle system frames.
			@remarks
				This causes a reset of all particles.
		*/
		void ClearFrames();

        /** Create a snapshot for a frame at index. */
        void GenerateSnapshot(ParticleSystemSnapshot& snapShot, u32 index) const;

		/** Resets all particles. */
		void Reset();
		
		/** Updates particles based on particle system frames. */
		void Update(f32 dt);

		/** Draws all particles immediately. */
		void Draw();

		/** Draws all particles in the render queue */
		void Draw(RenderQueue& renderQueue, u8 layer, const Simian::Matrix& transform = Simian::Matrix::Identity);
	};
}

#endif
