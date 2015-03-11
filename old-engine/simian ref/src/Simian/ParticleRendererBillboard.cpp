/************************************************************************/
/*!
\file		ParticleRendererBillboard.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/ParticleRendererBillboard.h"
#include "Simian/VertexBuffer.h"
#include "Simian/IndexBuffer.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/RenderQueue.h"
#include "Simian/ParticleSystem.h"
#include "Simian/VertexFormat.h"
#include "Simian/Camera.h"

namespace Simian
{
    ParticleRendererBillboard::ParticleRendererBillboard( ParticleSystem* parent )
        : ParticleRenderer(parent),
          vertexBuffer_(0),
          indexBuffer_(0),
          device_(0),
          camera_(0),
          material_(0),
          local_(false),
          velocityOrient_(false)
    {
    }

    //--------------------------------------------------------------------------

    GraphicsDevice* ParticleRendererBillboard::Device() const
    {
        return device_;
    }

    void ParticleRendererBillboard::Device( GraphicsDevice* val )
    {
        device_ = val;
    }

    Simian::Camera* ParticleRendererBillboard::Camera() const
    {
        return camera_;
    }

    void ParticleRendererBillboard::Camera( Simian::Camera* val )
    {
        camera_ = val;
    }

    Simian::Material* ParticleRendererBillboard::Material() const
    {
        return material_;
    }

    void ParticleRendererBillboard::Material( Simian::Material* val )
    {
        material_ = val;
    }

    bool ParticleRendererBillboard::Local() const
    {
        return local_;
    }

    void ParticleRendererBillboard::Local( bool val )
    {
        local_ = val;
    }

    bool ParticleRendererBillboard::VelocityOrient() const
    {
        return velocityOrient_;
    }

    void ParticleRendererBillboard::VelocityOrient( bool val )
    {
        velocityOrient_ = val;
    }

    //--------------------------------------------------------------------------

    void ParticleRendererBillboard::render_( DelegateParameter& )
    {
        Render();
    }

    //--------------------------------------------------------------------------

    void ParticleRendererBillboard::Load()
    {
        // create a vertex buffer big enough to store all the verts
        device_->CreateVertexBuffer(vertexBuffer_);

        // create vertex buffer and reserve vertices
        u32 numVerts = ParentSystem()->TotalParticles() * 4;
        vertexBuffer_->Load(PositionColorUVVertex::Format, numVerts, true);
        vertices_.reserve(numVerts); // reserve enough for max verts

        // create index buffer
        u32 numIndices = ParentSystem()->TotalParticles() * 6;
        std::vector<u16> indices;
        indices.reserve(numIndices);
        for (u16 i = 0; i < ParentSystem()->TotalParticles(); ++i)
        {
            indices.push_back(i * 4 + 0);
            indices.push_back(i * 4 + 1);
            indices.push_back(i * 4 + 2);
            indices.push_back(i * 4 + 2);
            indices.push_back(i * 4 + 3);
            indices.push_back(i * 4 + 0);
        }

        // create index buffer and load data into it
        device_->CreateIndexBuffer(indexBuffer_);
        indexBuffer_->Load(IndexBuffer::IBT_U16, numIndices);
        indexBuffer_->Data(&indices[0]);
    }

    void ParticleRendererBillboard::Unload()
    {
        vertexBuffer_->Unload();
        indexBuffer_->Unload();

        device_->DestroyVertexBuffer(vertexBuffer_);
        device_->DestroyIndexBuffer(indexBuffer_);
    }

    void ParticleRendererBillboard::Update( f32 )
    {
        // loop through all active particles and construct them so that they are facing the camera..

        // get the cameras lookat direction..
        Vector3 camDirection = Vector3::UnitZ;
        if (camera_)
        {
            camDirection = Vector3(camera_->View()[2][0],
                                   camera_->View()[2][1],
                                   camera_->View()[2][2]);
        }

        
        // compute crucial billboarding vars (from billboard matrix)
        /*
            | cz      -cxcy       cx |
            | 0       cz2 + cx2   cy |
            | -cx     -cycz       cz |
        */
        f32 cxcy = camDirection.X() * camDirection.Y();
        f32 cz2cx2 = camDirection.Z() * camDirection.Z() + camDirection.X() * camDirection.X();
        f32 cycz = camDirection.Y() * camDirection.Z();

        vertices_.clear();
        // loop through all active particles
        for (u32 i = 0; i < ParentSystem()->ActiveParticles().size(); ++i)
        {
            Particle* const& particle = ParentSystem()->ActiveParticles()[i];

            Vector3 right = Vector3::UnitX;
            if (velocityOrient_)
            {
                // orient based on velocity (cam direction forms the normal of 
                // the billboard plane)
                Simian::f32 d = particle->WorldVelocity.Dot(-camDirection);
                right = particle->WorldVelocity + d * camDirection;
            }
            else
            {
                // compute cos and sin
                f32 cosA = std::cos(particle->Rotation.Z());
                f32 sinA = std::sin(particle->Rotation.Z());

                // find the "right" vector
                right = Vector3(camDirection.Z() * cosA - cxcy * sinA,
                                cz2cx2 * sinA,
                                -camDirection.X() * cosA - cycz * sinA);
            }
            right.Normalize();

            // find the "up" vector
            Vector3 up = camDirection.Cross(right);

            f32 hSX = particle->Scale.X() * 0.5f;
            f32 hSY = particle->Scale.Y() * 0.5f;

            Vector3 position = particle->Position;
            position -= local_ ? particle->Origin : 0;

            // create the 4 verts
            // top left
            vertices_.push_back(Simian::PositionColorUVVertex(
                position + right * -hSX + up * hSY,
                particle->Color, Simian::Vector2(0, 0)));
            // bottom left
            vertices_.push_back(Simian::PositionColorUVVertex(
                position + right * -hSX + up * -hSY,
                particle->Color, Simian::Vector2(0, 1)));
            // bottom right
            vertices_.push_back(Simian::PositionColorUVVertex(
                position + right * hSX + up * -hSY,
                particle->Color, Simian::Vector2(1, 1)));
            // top right
            vertices_.push_back(Simian::PositionColorUVVertex(
                position + right * hSX + up * hSY,
                particle->Color, Simian::Vector2(1, 0)));
        }

        if (vertices_.size())
            vertexBuffer_->Data(&vertices_[0], 
                vertices_.size() * PositionColorUVVertex::Format.Size());
    }

    void ParticleRendererBillboard::Render()
    {
        if (vertices_.size())
        {
            // HACK: This should be part of the material
            device_->RenderState(GraphicsDevice::RS_DEPTHWRITE, false);
            device_->RenderState(GraphicsDevice::RS_DEPTHENABLED, true);
            // ENDHACK

            // particles are in world space if they are not local
            if (!local_)
                device_->World(Simian::Matrix::Identity);
            
            // draw the vertex buffer
            device_->DrawIndexed(vertexBuffer_, indexBuffer_, 
                GraphicsDevice::PT_TRIANGLES, vertices_.size()/2);
        }
    }

    void ParticleRendererBillboard::Render( RenderQueue& renderQueue, u8 layer, const Simian::Matrix& transform )
    {
        renderQueue.AddRenderObject(RenderObject(*material_, layer, transform, 
            Delegate::CreateDelegate<ParticleRendererBillboard, &ParticleRendererBillboard::render_>(this), 
            NULL));
    }
}
