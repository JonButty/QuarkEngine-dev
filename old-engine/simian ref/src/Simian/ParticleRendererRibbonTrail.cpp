/************************************************************************/
/*!
\file		ParticleRendererRibbonTrail.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/ParticleRendererRibbonTrail.h"

#include <algorithm>
#include <functional>
#include "Simian/GraphicsDevice.h"
#include "Simian/ParticleSystem.h"
#include "Simian/IndexBuffer.h"
#include "Simian/Math.h"
#include "Simian/RenderQueue.h"

namespace Simian
{
    ParticleRendererRibbonTrail::ParticleRendererRibbonTrail( ParticleSystem* parent )
        : ParticleRenderer(parent),
          vertexBuffer_(0),
          indexBuffer_(0),
          device_(0),
          material_(0),
          segmentCount_(2),
          interpolationInterval_(0.1f),
          segmentTwist_(0.0f),
          segmentTwistVelocity_(0.0f),
          trailCount_(0),
          local_(false)
    {
    }

    //--------------------------------------------------------------------------

    GraphicsDevice* ParticleRendererRibbonTrail::Device() const
    {
        return device_;
    }

    void ParticleRendererRibbonTrail::Device( GraphicsDevice* val )
    {
        device_ = val;
    }

    Simian::Material* ParticleRendererRibbonTrail::Material() const
    {
        return material_;
    }

    void ParticleRendererRibbonTrail::Material( Simian::Material* val )
    {
        material_ = val;
    }

    void ParticleRendererRibbonTrail::SegmentCount( u32 segments )
    {
        segmentCount_ = segments;
    }

    void ParticleRendererRibbonTrail::InterpolationInterval( Simian::f32 val )
    {
        interpolationInterval_ = val;
    }

    void ParticleRendererRibbonTrail::SegmentTwist( f32 twist )
    {
        segmentTwist_ = twist;
    }

    void ParticleRendererRibbonTrail::SegmentTwistVelocity( f32 velocity )
    {
        segmentTwistVelocity_ = velocity;
    }

    bool ParticleRendererRibbonTrail::Local() const
    {
        return local_;
    }

    void ParticleRendererRibbonTrail::Local( bool val )
    {
        local_ = val;
    }

    //--------------------------------------------------------------------------

    void ParticleRendererRibbonTrail::render_( Simian::DelegateParameter& )
    {
        Render();
    }

    //--------------------------------------------------------------------------

    void ParticleRendererRibbonTrail::Load()
    {
        // set up all the trails
        trails_.resize(ParentSystem()->TotalParticles(), Trail(segmentCount_));
        for (u32 i = 0; i < trails_.size(); ++i)
        {
            trails_[i].InterpolationInterval(interpolationInterval_);
            trails_[i].SegmentTwist(segmentTwist_);
            trails_[i].SegmentTwistVelocity(segmentTwistVelocity_);
            trails_[i].Reset();
        }

        // create the vertex buffer
        device_->CreateVertexBuffer(vertexBuffer_);

        u32 numVerts = ParentSystem()->TotalParticles() * 2 * segmentCount_;
        vertexBuffer_->Load(PositionColorUVVertex::Format, numVerts, true);
        vertices_.clear();
        vertices_.reserve(numVerts);

        // create the index buffer
        device_->CreateIndexBuffer(indexBuffer_);

        u32 numIndices = ParentSystem()->TotalParticles() * (segmentCount_ * 6 - 6);
        indexBuffer_->Load(IndexBuffer::IBT_U16, numIndices);
        std::vector<u16> indices;
        indices.reserve(numIndices);
        for (u32 i = 0; i < ParentSystem()->TotalParticles(); ++i)
        {
            u32 start = i * segmentCount_ * 2;
            for (u32 j = 0; j < segmentCount_ - 1; ++j)
            {
                u16 offset = static_cast<u16>(start + j * 2);
                indices.push_back(offset + 0);
                indices.push_back(offset + 1);
                indices.push_back(offset + 2);
                indices.push_back(offset + 2);
                indices.push_back(offset + 1);
                indices.push_back(offset + 3);
            }
        }
        indexBuffer_->Data(&indices[0]);
    }

    void ParticleRendererRibbonTrail::Unload()
    {
        indexBuffer_->Unload();
        vertexBuffer_->Unload();

        device_->DestroyIndexBuffer(indexBuffer_);
        device_->DestroyVertexBuffer(vertexBuffer_);
    }

    void ParticleRendererRibbonTrail::Update( f32 dt )
    {
        // update vertices for each ribbon trail
        trailCount_ = 0;
        vertices_.clear();

        for (u32 i = 0; i < ParentSystem()->ActiveParticles().size(); ++i)
        {
            Particle* const particle = ParentSystem()->ActiveParticles()[i];

            // update the corresponding trail
            Trail& trail = trails_[particle->ParticleId];
            Vector3 position = (particle->Position - particle->Origin);
            position = !local_ ? ParentSystem()->Transform().Transform(position) : position;
            trail.Position(position);
            trail.Twist(particle->Rotation.Z());

            trail.Update(dt);

            // update vertex buffer with the trail information
            const std::vector<Trail::Segment>& segments = trails_[particle->ParticleId].Segments();
            for (u32 j = 0; j < segments.size(); ++j)
            {
                Vector3 axis;
                if (j != 0)
                    axis += segments[j].Position - segments[j - 1].Position;
                if (j != segments.size() - 1)
                    axis += segments[j + 1].Position - segments[j].Position;

                Vector3 right;
                if (axis.LengthSquared() > Simian::Math::EPSILON)
                    right = Matrix::RotationAxisAngle(axis, Simian::Radian(segments[j].Twist)) * Vector3::UnitX;

                f32 u = j/(segments.size() - 1.0f);
                vertices_.push_back(PositionColorUVVertex(
                    segments[j].Position + right * particle->Scale.Y(),
                    particle->Color, Simian::Vector2(u, 0.0f)));
                vertices_.push_back(PositionColorUVVertex(
                    segments[j].Position - right * particle->Scale.Y(),
                    particle->Color, Simian::Vector2(u, 1.0f)));
            }

            ++trailCount_;
        }

        if (vertices_.size())
            vertexBuffer_->Data(&vertices_[0],
                vertices_.size() * PositionColorUVVertex::Format.Size());
    }

    void ParticleRendererRibbonTrail::Render()
    {
        if (trailCount_)
        {
            if (!local_)
                device_->World(Simian::Matrix::Identity);

            // draw the vertex buffer
            device_->DrawIndexed(vertexBuffer_, indexBuffer_, 
                GraphicsDevice::PT_TRIANGLES, trailCount_ * (segmentCount_ - 1) * 2);
        }
    }

    void ParticleRendererRibbonTrail::Render( RenderQueue& renderQueue, u8 layer, const Simian::Matrix& transform )
    {
        renderQueue.AddRenderObject(RenderObject(*material_, layer, transform, 
            Delegate::CreateDelegate<ParticleRendererRibbonTrail, &ParticleRendererRibbonTrail::render_>(this), 
            NULL));
    }

    void ParticleRendererRibbonTrail::OnSpawn( Particle& particle )
    {
        trails_[particle.ParticleId].Position(particle.Position - (local_ ? particle.Origin : 0));
        trails_[particle.ParticleId].Twist(particle.Rotation.Z());
        trails_[particle.ParticleId].Reset();
    }
}
