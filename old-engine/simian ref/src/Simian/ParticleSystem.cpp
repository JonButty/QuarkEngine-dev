/************************************************************************/
/*!
\file		ParticleSystem.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/ParticleSystem.h"
#include "Simian/ParticleRenderer.h"
#include "Simian/RenderQueue.h"
#include "Simian/Math.h"
#include "Simian/ParticleEmitter.h"
#include "Simian/Interpolation.h"
#include "Simian/LogManager.h"

namespace Simian
{
    ParticleSystem::ParticleSystem()
        : renderer_(0),
          emitter_(0),
          spawnInterval_(0),
          randomSpawnInterval_(0),
          spawnAmount_(0),
          randomSpawnAmount_(0),
          spawnTimer_(0),
          lifeTime_(1.0f),
          randomLifeTime_(0.0f),
          enabled_(true)
    {
    }

    //--------------------------------------------------------------------------

    Simian::u32 ParticleSystem::TotalParticles() const
    {
        return particles_.size();
    }

    void ParticleSystem::SpawnInterval(f32 val)
    {
        spawnInterval_ = val;
    }

    f32 ParticleSystem::SpawnInterval() const
    {
        return spawnInterval_;
    }

    void ParticleSystem::RandomSpawnInterval(f32 val)
    {
        randomSpawnInterval_ = val;
    }

    f32 ParticleSystem::RandomSpawnInterval() const
    {
        return randomSpawnInterval_;
    }

    void ParticleSystem::SpawnAmount(u32 val)
    {
        spawnAmount_ = val;
    }

    u32 ParticleSystem::SpawnAmount() const
    {
        return spawnAmount_;
    }

    void ParticleSystem::RandomSpawnAmount(u32 val)
    {
        randomSpawnAmount_ = val;
    }

    u32 ParticleSystem::RandomSpawnAmount() const
    {
        return randomSpawnAmount_;
    }
    
    void ParticleSystem::LifeTime(f32 val)
    {
        lifeTime_ = val;
    }

    f32 ParticleSystem::LifeTime() const
    {
        return lifeTime_;
    }

    void ParticleSystem::RandomLifeTime(f32 val)
    {
        randomLifeTime_ = val;
    }

    f32 ParticleSystem::RandomLifeTime() const
    {
        return randomLifeTime_;
    }

    const Matrix& ParticleSystem::Transform() const
    {
        return transform_;
    }

    void ParticleSystem::Transform( const Matrix& val )
    {
        transform_ = val;
    }

    bool ParticleSystem::Enabled() const
    {
        return enabled_;
    }

    void ParticleSystem::Enabled( bool val )
    {
        enabled_ = val;
    }

    const std::vector<Particle*>& ParticleSystem::ActiveParticles() const
    {
        return activeParticles_;
    }

    const std::vector<Particle>& ParticleSystem::Particles() const
    {
        return particles_;
    }

    //--------------------------------------------------------------------------
    
    void ParticleSystem::initParticle_(Particle& particle)
    {
        // create the very first snapshot.
        // if no frames exist.. this is bad and will probably crash
        GenerateSnapshot(particle.Current, 0);
        GenerateSnapshot(particle.Next, 1 < frames_.size() ? 1 : 0);
        particle.FrameIndex = 1;

        emitter_->Spawn(particle);

        // set up all properties for the particle except position
        particle.Time = 0.0f;
        particle.LifeTime = lifeTime_ + Math::Random(0.0f, randomLifeTime_);
        particle.Velocity = particle.Current.Velocity;
        particle.Origin = transform_.Position();
        particle.Scale = particle.Current.Scale;
        particle.Rotation = particle.Current.Rotation;
        particle.AngularVelocity = particle.Current.AngularVelocity;
        particle.Color = particle.Current.Color;

        // inform renderer that the particle is spawned
        renderer_->OnSpawn(particle);
    }

    //--------------------------------------------------------------------------

    void ParticleSystem::Load(u32 totalParticles)
    {
        particles_.resize(totalParticles);
        activeParticles_.reserve(totalParticles);
        freeParticles_.reserve(totalParticles);
        Reset();

        // load the renderer..
        if (renderer_)
            renderer_->Load();
    }

    void ParticleSystem::Unload()
    {
        // unload the renderer..
        renderer_->Unload();

        // destroy emitter and renderer
        delete renderer_;
        renderer_ = 0;

        delete emitter_;
        emitter_ = 0;
    }

    void ParticleSystem::AddFrame(const ParticleSystemFrame& frame)
    {
        Reset();

        // find a place to insert the frame.
        std::vector<ParticleSystemFrame>::iterator next = frames_.begin();
        while (next != frames_.end())
        {
            if (next->Time > frame.Time)
                break;
            ++next;
        }

        // boundary case just insert
        if (next == frames_.begin() || next == frames_.end())
        {
            frames_.insert(next, frame);
            return;
        }

        // find previous
        std::vector<ParticleSystemFrame>::iterator prev = next - 1;

        // copy frame
        ParticleSystemFrame newFrame = frame;
        
        // find intp
        f32 intp = (frame.Time - prev->Time)/(next->Time - prev->Time);

#define INTERPOLATE_CAP(cap, arg) \
        if (~newFrame.Caps & cap) \
            newFrame.arg = Interpolation::Interpolate(prev->arg, next->arg, intp, Interpolation::Linear);

        // if caps doesn't exist, then do interpolation
        INTERPOLATE_CAP(FC_VELOCITY, Velocity);
        INTERPOLATE_CAP(FC_RANDOMVELOCITY, RandomVelocity);
        INTERPOLATE_CAP(FC_TANGENTVELOCITY, TangentVelocity);
        INTERPOLATE_CAP(FC_RANDOMTANGENTVELOCITY, RandomTangentVelocity);
        INTERPOLATE_CAP(FC_SCALE, Scale);
        INTERPOLATE_CAP(FC_RANDOMSCALE, RandomScale);
        INTERPOLATE_CAP(FC_ROTATION, Rotation);
        INTERPOLATE_CAP(FC_RANDOMROTATION, RandomRotation);
        INTERPOLATE_CAP(FC_ANGULARVELOCITY, AngularVelocity);
        INTERPOLATE_CAP(FC_RANDOMANGULARVELOCITY, RandomAngularVelocity);
        INTERPOLATE_CAP(FC_COLOR, Color);
        INTERPOLATE_CAP(FC_RANDOMCOLOR, RandomColor);

#undef INTERPOLATE_CAP

        frames_.insert(next, newFrame);
    }

    const std::vector<ParticleSystemFrame>& ParticleSystem::Frames() const
    {
        return frames_;
    }

    void ParticleSystem::ClearFrames()
    {
        Reset();
        frames_.clear();
    }
    
    void ParticleSystem::GenerateSnapshot(ParticleSystemSnapshot& snapShot, u32 index) const
    {
        snapShot.Time = frames_[index].Time;
        snapShot.Velocity = frames_[index].Velocity + 
            Simian::Math::RandomPerElement(-frames_[index].RandomVelocity, frames_[index].RandomVelocity, 3);
        snapShot.Scale = frames_[index].Scale + 
            Simian::Math::Random(-frames_[index].RandomScale, frames_[index].RandomScale);
        snapShot.Rotation = frames_[index].Rotation + 
            Simian::Math::RandomPerElement(-frames_[index].RandomRotation, frames_[index].RandomRotation, 3);
        snapShot.TangentVelocity = frames_[index].TangentVelocity + 
            Simian::Math::RandomPerElement(-frames_[index].RandomTangentVelocity, frames_[index].RandomTangentVelocity, 3);
        snapShot.AngularVelocity = frames_[index].AngularVelocity + 
            Simian::Math::RandomPerElement(-frames_[index].RandomAngularVelocity, frames_[index].RandomAngularVelocity, 3);
        snapShot.Color = frames_[index].Color + 
            Simian::Math::Random(-frames_[index].RandomColor, frames_[index].RandomColor);
    }

    void ParticleSystem::Reset()
    {
        activeParticles_.clear();
        freeParticles_.clear();
        for (u32 i = 0; i < particles_.size(); ++i)
        {
            particles_[i].ParticleId = i;
            freeParticles_.push_back(&particles_[i]);
        }
    }

    void ParticleSystem::Update(f32 dt)
    {
        // trivial rejection cases
        if (!emitter_ || !renderer_ || !frames_.size() || !spawnAmount_)
            return;

        // update the frame and interpolate all parameters (generate snapshot)
        u32 i = 0;
        while (i < activeParticles_.size())
        {
            Particle& particle = *activeParticles_[i];

            // increment time
            particle.Time += dt;

            // find normalized time
            f32 intp = particle.Time/particle.LifeTime;

            // if intp > 1, then particle is dead
            if (intp > 1)
            {
                std::vector<Particle*>::iterator iter = activeParticles_.begin();
                iter += i;
                activeParticles_.erase(iter);
                freeParticles_.push_back(&particle);
                // don't increment i so we do the same index again
                continue;
            }

            // check if we passed the particles next frame
            while (intp > particle.Next.Time && particle.FrameIndex < frames_.size() - 1)
            {
                ++particle.FrameIndex;
                particle.Current = particle.Next;
                GenerateSnapshot(particle.Next, particle.FrameIndex);
            }

            // interpolate only when at least 2 frames exist.
            Vector3 tangentVelocity;
            if (frames_.size() > 1)
            {
                intp -= particle.Current.Time;
                intp /= particle.Next.Time - particle.Current.Time;

                // interpolate to get the properties
                particle.Velocity = Simian::Interpolation::Interpolate(particle.Current.Velocity, particle.Next.Velocity, intp, Simian::Interpolation::Linear);
                // interpolate tangent velocity
                tangentVelocity = Simian::Interpolation::Interpolate(particle.Current.TangentVelocity, particle.Next.TangentVelocity, intp, Simian::Interpolation::Linear);
                // compute other attributes
                particle.Scale = Simian::Interpolation::Interpolate(particle.Current.Scale, particle.Next.Scale, intp, Simian::Interpolation::Linear);
                particle.AngularVelocity = Simian::Interpolation::Interpolate(particle.Current.AngularVelocity, particle.Next.AngularVelocity, intp, Simian::Interpolation::Linear);
                particle.Color = Simian::Interpolation::Interpolate(particle.Current.Color, particle.Next.Color, intp, Simian::Interpolation::Linear);
            }

            // transform velocity into world space
            Vector4 velocity(particle.Velocity, 0.0f);

            // scale by time
            velocity *= dt;

            // add tangent velocity
            tangentVelocity *= dt;
            Vector3 dist = particle.Position - particle.Origin;
            f32 radius = dist.Length();
            f32 angleX = tangentVelocity.Y()/radius;
            f32 angleY = tangentVelocity.X()/radius;
            Matrix rot(Matrix::RotationAxisAngle(Vector3::UnitY, Simian::Radian(angleY)) *
                Matrix::RotationAxisAngle(Vector3::UnitX, Simian::Radian(angleX)));
            Vector3 newDist(dist);
            rot.Transform(newDist);
            newDist += newDist * tangentVelocity.Z()/radius;
            Vector3 tangentVel(newDist - dist);
            velocity += tangentVel;

            // set the world velocity
            particle.WorldVelocity = Simian::Vector3(velocity.X(), velocity.Y(), 
                                                     velocity.Z());

            // apply rotation
            transform_.Transform(velocity);

            // integrate velocity
            particle.Position += Vector3(velocity.X(), velocity.Y(), velocity.Z());

            // integrate angular velocity
            particle.Rotation += particle.AngularVelocity * dt;

            ++i;
        }

        // spawn particles if its time and the particle system is enabled
        spawnTimer_ -= dt;
        if (spawnTimer_ < 0.0f && enabled_)
        {
            // spawn particles
            u32 numParticles = spawnAmount_ + 
                (randomSpawnAmount_ ? Simian::Math::Random(0, randomSpawnAmount_) : 0);

            for (u32 i = 0; i < numParticles; ++i)
            {
                // no more particles.. don't try and spawn em
                if (!freeParticles_.size())
                    break;

                // get a free particle
                Particle* particle = freeParticles_.back();
                freeParticles_.pop_back();

                // spawn it
                initParticle_(*particle);

                // then put it on the active particles list
                activeParticles_.push_back(particle);
            }

            // randomize spawn timer
            spawnTimer_ = spawnInterval_ + Simian::Math::Random(0.0f, randomSpawnInterval_);
        }

        // update the renderer
        renderer_->Update(dt);
    }

    void ParticleSystem::Draw()
    {
        renderer_->Render();
    }

    void ParticleSystem::Draw(RenderQueue& renderQueue, u8 layer, const Simian::Matrix& transform)
    {
        renderer_->Render(renderQueue, layer, transform);
    }
}
