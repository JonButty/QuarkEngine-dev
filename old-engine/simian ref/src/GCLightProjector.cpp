/*************************************************************************/
/*!
\file		GCLightProjector.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCLightProjector.h"
#include "ComponentTypes.h"

#include "Simian/EnginePhases.h"
#include "Simian/CTransform.h"
#include "Simian/COrthoCamera.h"
#include "Simian/Game.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/Scene.h"
#include "Simian/MaterialCache.h"
#include "Simian/VertexBuffer.h"
#include "Simian/RenderQueue.h"

namespace Descension
{
    const Simian::f32 EDGE_SIZE = 3.0f;

    Simian::FactoryPlant<Simian::EntityComponent, GCLightProjector> GCLightProjector::factoryPlant_ =
        Simian::FactoryPlant<Simian::EntityComponent, GCLightProjector>(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_LIGHTPROJECTOR);

    GCLightProjector::GCLightProjector()
        : projector_(0),
          transform_(0),
          cameraTransform_(0),
          ambient_(0.2f, 0.2f, 0.2f),
          edge_(0.025f, 0.025f, 0.025f),
          projectionSize_(6.0f),
          textureMatrixIndex_(1)
    {
    }

    //--------------------------------------------------------------------------

    const Simian::Color& GCLightProjector::Ambient() const
    {
        return ambient_;
    }

    void GCLightProjector::Ambient( const Simian::Color& val )
    {
        ambient_ = val;
        projector_->ClearColor(ambient_);
    }

    const Simian::Color& GCLightProjector::Edge() const
    {
        return edge_;
    }

    void GCLightProjector::Edge( const Simian::Color& val )
    {
        edge_ = val;
    }

    //--------------------------------------------------------------------------

    void GCLightProjector::updateTextureMatrix_( Simian::DelegateParameter& )
    {
        // update the texture matrix
        Simian::f32 minX = transform_->World().Position().X() - projectionSize_ * 0.5f;
        Simian::f32 maxY = transform_->World().Position().Z() + projectionSize_ * 0.5f;

        Simian::Game::Instance().GraphicsDevice().TextureMatrix(textureMatrixIndex_, 
            Simian::Matrix(
                1, 0, 0, -minX,
                0, 0, -1, maxY,
                0, 0, 0, projectionSize_,
                0, 0, 0, 1) * cameraTransform_->World());
    }

    void GCLightProjector::drawEdges_( Simian::DelegateParameter& )
    {
        // draw the edges
        Simian::f32 hsize = projectionSize_ * 0.5f;
        Simian::f32 hedge = EDGE_SIZE * 0.5f;
        
        // get world
        Simian::Matrix projectorMat = 
            Simian::Matrix::Translation(transform_->World().Position());
        Simian::Matrix world = 
            Simian::Matrix::Translation(Simian::Vector3(hedge - hsize, -1.1f, 0)) *
            Simian::Matrix::Scale(EDGE_SIZE, 1.0f, projectionSize_);

        Simian::Game::Instance().GraphicsDevice().World(projectorMat * world);
        Simian::Game::Instance().GraphicsDevice().Draw(unitQuad_, Simian::GraphicsDevice::PT_TRIANGLESTRIP, 2);

        // create rotation (draw last 3)
        Simian::Matrix rot = Simian::Matrix::RotationAxisAngle(Simian::Vector3::UnitY, Simian::Degree(90.0f));
        world = rot * world;
        Simian::Game::Instance().GraphicsDevice().World(projectorMat * world);
        Simian::Game::Instance().GraphicsDevice().Draw(unitQuad_, Simian::GraphicsDevice::PT_TRIANGLESTRIP, 2);

        world = rot * world;
        Simian::Game::Instance().GraphicsDevice().World(projectorMat * world);
        Simian::Game::Instance().GraphicsDevice().Draw(unitQuad_, Simian::GraphicsDevice::PT_TRIANGLESTRIP, 2);

        world = rot * world;
        Simian::Game::Instance().GraphicsDevice().World(projectorMat * world);
        Simian::Game::Instance().GraphicsDevice().Draw(unitQuad_, Simian::GraphicsDevice::PT_TRIANGLESTRIP, 2);
    }

    void GCLightProjector::render_( Simian::DelegateParameter& )
    {
        Simian::Game::Instance().RenderQueue().AddRenderObject(Simian::RenderObject(
            *edgeMaterial_, Simian::RenderQueue::RL_PRESCENE, transform_->World(),
            Simian::Delegate::CreateDelegate<GCLightProjector, &GCLightProjector::drawEdges_>(this),
            0));
    }

    //--------------------------------------------------------------------------

    void GCLightProjector::OnSharedLoad()
    {
        edgeMaterial_ = Simian::Game::Instance().MaterialCache().Load("Materials/LightProjectorEdge.mat");

        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCLightProjector, &GCLightProjector::updateTextureMatrix_>(this));
        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCLightProjector, &GCLightProjector::render_>(this));
    }

    void GCLightProjector::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_ORTHOCAMERA, projector_);

        Simian::Entity* entity = ParentScene()->EntityByNameRecursive("Camera");
        entity->ComponentByType(Simian::C_TRANSFORM, cameraTransform_);

        projector_->ClearColor(ambient_);
        projector_->Size(Simian::Vector2(projectionSize_, projectionSize_));

        // create the unit quad (oriented upwards)
        Simian::Game::Instance().GraphicsDevice().CreateVertexBuffer(unitQuad_);
        unitQuad_->Load(Simian::PositionColorUVVertex::Format, 4);
        Simian::PositionColorUVVertex vertices[4] = {
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, 0, -0.5f), edge_, Simian::Vector2(0, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, 0, -0.5f), edge_, Simian::Vector2(1, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, 0, 0.5f), edge_, Simian::Vector2(0, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, 0, 0.5f), edge_, Simian::Vector2(1, 0))
        };
        unitQuad_->Data(vertices);
    }

    void GCLightProjector::OnDeinit()
    {
        unitQuad_->Unload();
        Simian::Game::Instance().GraphicsDevice().DestroyVertexBuffer(unitQuad_);
    }

    void GCLightProjector::Deserialize( const Simian::FileObjectSubNode& data )
    {
        const Simian::FileObjectSubNode* ambient = data.Object("Ambient");
        if (ambient)
        {
            const Simian::FileDataSubNode* r = ambient->Data("R");
            const Simian::FileDataSubNode* g = ambient->Data("G");
            const Simian::FileDataSubNode* b = ambient->Data("B");

            ambient_ = Simian::Color(
                r ? r->AsF32() : ambient_.R(),
                g ? g->AsF32() : ambient_.G(),
                b ? b->AsF32() : ambient_.B());
        }

        const Simian::FileObjectSubNode* edge = data.Object("Edge");
        if (edge)
        {
            float r, g, b;
            edge->Data("R", r, edge_.R());
            edge->Data("G", g, edge_.G());
            edge->Data("B", b, edge_.B());
            edge_ = Simian::Color(r, g, b);
        }

        const Simian::FileDataSubNode* projectionSize = data.Data("ProjectionSize");
        projectionSize_ = projectionSize ? projectionSize->AsF32() : projectionSize_;

        const Simian::FileDataSubNode* textureMatrixIndex = data.Data("TextureMatrixIndex");
        textureMatrixIndex_ = textureMatrixIndex ? static_cast<Simian::u32>(textureMatrixIndex->AsF32()) : textureMatrixIndex_;
    }

    void GCLightProjector::Serialize( Simian::FileObjectSubNode& data )
    {
        Simian::FileObjectSubNode* ambient = data.AddObject("Ambient");
        ambient->AddData("R", ambient_.R());
        ambient->AddData("G", ambient_.G());
        ambient->AddData("B", ambient_.B());

        Simian::FileObjectSubNode* edge = data.AddObject("Edge");
        edge->AddData("R", edge_.R());
        edge->AddData("G", edge_.G());
        edge->AddData("B", edge_.B());

        data.AddData("ProjectionSize", projectionSize_);
        data.AddData("TextureMatrixIndex", textureMatrixIndex_);
    }
}
