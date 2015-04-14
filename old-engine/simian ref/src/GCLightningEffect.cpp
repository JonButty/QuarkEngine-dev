/*************************************************************************/
/*!
\file		GCLightningEffect.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCLightningEffect.h"
#include "ComponentTypes.h"

#include "Simian/EnginePhases.h"
#include "Simian/Game.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/IndexBuffer.h"
#include "Simian/Camera.h"
#include "Simian/Math.h"
#include "Simian/RenderQueue.h"
#include "Simian/MaterialCache.h"
#include "Simian/TextureCache.h"
#include "Simian/CTransform.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCLightningEffect> GCLightningEffect::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_LIGHTNINGEFFECT);

    GCLightningEffect::GCLightningEffect()
        : transform_(0),
          vertexBuffer_(0),
          indexBuffer_(0),
          generations_(5),
          deviation_(0.2f),
          thickness_(0.3f),
          flickerDuration_(0.01f),
          timer_(0.0f),
          forkChance_(0.3f),
          forkScale_(0.7f),
          deviationDecay_(0.5f),
          colorDecay_(0.4f),
          material_(0),
          visible_(true)
    {
    }
    
    //--------------------------------------------------------------------------

    const Simian::Vector3& GCLightningEffect::Origin() const
    {
        return origin_;
    }

    void GCLightningEffect::Origin( const Simian::Vector3& val )
    {
        origin_ = val;
    }

    const Simian::Vector3& GCLightningEffect::Target() const
    {
        return target_;
    }

    void GCLightningEffect::Target( const Simian::Vector3& val )
    {
        target_ = val;
    }

    bool GCLightningEffect::Visible() const
    {
        return visible_;
    }

    void GCLightningEffect::Visible( bool val )
    {
        visible_ = val;
        if (visible_)
        {
            generatePoints_();
            updateVertices_();
        }
    }

    //--------------------------------------------------------------------------

    void GCLightningEffect::update_( Simian::DelegateParameter& param )
    {
        Simian::EntityUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;
        if (timer_ > flickerDuration_)
        {
            // generate lightning points
            generatePoints_();

            // create the strips for the lightning points
            updateVertices_();

            timer_ = 0.0f;
        }
    }

    void GCLightningEffect::render_( Simian::DelegateParameter& )
    {
        // submit draw
        if (!vertices_.size() || !visible_)
            return;

        Simian::Game::Instance().RenderQueue().AddRenderObject(Simian::RenderObject(
            *material_, layer_, Simian::Matrix::Translation(target_), 
            Simian::Delegate::CreateDelegate<GCLightningEffect, &GCLightningEffect::draw_>(this),
            0));
    }

    void GCLightningEffect::draw_( Simian::DelegateParameter& )
    {
        // draw the lightning!
        Simian::Game::Instance().GraphicsDevice().World(
            transform_ ? transform_->World() : Simian::Matrix::Identity);
        Simian::Game::Instance().GraphicsDevice().DrawIndexed(
            vertexBuffer_, indexBuffer_, Simian::GraphicsDevice::PT_TRIANGLES, 
            vertices_.size()/2);
    }

    void GCLightningEffect::generatePoints_()
    {
        lightningPoints_.clear();
        
        Simian::f32 currDev = deviation_;

        // insert segment
        LightningSegment segment;
        segment.From = origin_;
        segment.To = target_;
        segment.FromRight = (target_ - origin_).Normalized().Cross(camera_->Direction()).Normalized();
        segment.ToRight = segment.FromRight;
        lightningPoints_.push_back(segment);

        for (Simian::u32 i = 0; i < generations_; ++i)
        {
            // for each segment..
            for (Simian::u32 i = 0; i < lightningPoints_.size(); ++i)
            {
                // find the vector that passes through the lightning points
                Simian::Vector3 rotAxis = lightningPoints_[i].To - lightningPoints_[i].From;
                rotAxis.Normalize();
                Simian::Vector3 transVec = rotAxis.Cross(camera_->Direction().Normalized());
                transVec.Normalized();
                
                // randomize angle and length
                Simian::f32 angle = Simian::Math::Random(0.0f, Simian::Math::TAU);

                // find the midpoint position
                Simian::Vector3 midpoint = 0.5f * (lightningPoints_[i].From + lightningPoints_[i].To);

                // transforms a zero point
                Simian::Matrix transform = 
                    Simian::Matrix::Translation(midpoint) *
                    Simian::Matrix::RotationAxisAngle(rotAxis, Simian::Radian(angle)) *
                    Simian::Matrix::Translation(currDev * transVec);

                // this is where the point should be in world
                midpoint = transform.Position();

                segment.From = midpoint;
                segment.To = lightningPoints_[i].To;
                segment.FromRight = (segment.To - segment.From).Normalized().Cross(camera_->Direction()).Normalized();
                segment.ToRight = lightningPoints_[i].ToRight;
                segment.Color = lightningPoints_[i].Color;

                lightningPoints_[i].To = midpoint;
                lightningPoints_[i].ToRight = segment.FromRight;

                // increase the iterator for the newly inserted point
                ++i;

                // insert after first lightning point
                lightningPoints_.insert(lightningPoints_.begin() + i, segment);

                if (Simian::Math::Random(0.0f, 1.0f) < forkChance_)
                {
                    // create a fork
                    Simian::Vector3 direction = midpoint - lightningPoints_[i - 1].From;
                    
                    // get length and normalize
                    Simian::f32 length = direction.Length();
                    direction /= length;

                    // get fork length
                    Simian::f32 forkLength = forkScale_ * length;
                    segment.From = midpoint;
                    segment.To = midpoint + forkLength * direction;
                    segment.ToRight = (segment.To - segment.From).Normalized().Cross(camera_->Direction()).Normalized();
                    segment.Color = Simian::Color(1.0f, 1.0f, 1.0f,
                        lightningPoints_[i - 1].Color.A() * colorDecay_);

                    ++i;

                    // insert after next lightning point
                    lightningPoints_.insert(lightningPoints_.begin() + i, segment);
                }
            }

            currDev *= deviationDecay_;
        }
    }

    void GCLightningEffect::updateVertices_()
    {
        vertices_.clear();

        // create quads for each lightning piece
        for (Simian::u32 i = 0; i < lightningPoints_.size(); ++i)
        {
            // find the 4 vertices
            vertices_.push_back(Simian::PositionColorUVVertex(
                lightningPoints_[i].From - lightningPoints_[i].FromRight * thickness_,
                lightningPoints_[i].Color, Simian::Vector2(0, 0)));
            vertices_.push_back(Simian::PositionColorUVVertex(
                lightningPoints_[i].To - lightningPoints_[i].ToRight * thickness_,
                lightningPoints_[i].Color, Simian::Vector2(0, 1)));
            vertices_.push_back(Simian::PositionColorUVVertex(
                lightningPoints_[i].To + lightningPoints_[i].ToRight * thickness_,
                lightningPoints_[i].Color, Simian::Vector2(1, 1)));
            vertices_.push_back(Simian::PositionColorUVVertex(
                lightningPoints_[i].From + lightningPoints_[i].FromRight * thickness_,
                lightningPoints_[i].Color, Simian::Vector2(1, 0)));
        }

        if (vertices_.size())
            vertexBuffer_->Data(&vertices_[0], vertices_.size() * Simian::PositionColorUVVertex::Format.Size(), 0);
    }

    //--------------------------------------------------------------------------

    void GCLightningEffect::OnSharedLoad()
    {
        // load the material
        material_ = Simian::Game::Instance().MaterialCache().Load(materialFile_);
        Simian::Game::Instance().TextureCache().LoadFiles();
        Simian::Game::Instance().TextureCache().LoadTextures();

        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCLightningEffect, &GCLightningEffect::update_>(this));
        RegisterCallback(Simian::P_RENDER, 
            Simian::Delegate::CreateDelegate<GCLightningEffect, &GCLightningEffect::render_>(this));
    }

    void GCLightningEffect::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);

        // generations
        // 0 -> 2 points
        // 1 -> 4 points
        // 2 -> 10 points
        // n -> 1 + 3^n points

        // determine number of vertices using above formula
        Simian::u32 numVerts = 1;
        for (Simian::u32 i = 0; i < generations_; ++i)
            numVerts *= 3;
        numVerts *= 4;
        
        Simian::Game::Instance().GraphicsDevice().CreateVertexBuffer(vertexBuffer_);
        vertexBuffer_->Load(Simian::PositionColorUVVertex::Format, numVerts, true);
        vertices_.reserve(numVerts);

        // create index buffer (every 4 verts is a quad)
        Simian::u32 numIndicies = numVerts/4 * 6;
        
        Simian::Game::Instance().GraphicsDevice().CreateIndexBuffer(indexBuffer_);
        indexBuffer_->Load(Simian::IndexBuffer::IBT_U16, numIndicies);

        // fill the index buffer
        std::vector<Simian::u16> indices_;
        indices_.reserve(numIndicies);
        for (Simian::u16 i = 0; i < numVerts/4; ++i)
        {
            indices_.push_back(i * 4 + 0);
            indices_.push_back(i * 4 + 1);
            indices_.push_back(i * 4 + 2);
            indices_.push_back(i * 4 + 2);
            indices_.push_back(i * 4 + 3);
            indices_.push_back(i * 4 + 0);
        }

        // set the index buffer
        indexBuffer_->Data(&indices_[0]);

        // find a camera in the same layer
        for (Simian::u32 i = 0; i < Simian::Game::Instance().RenderQueue().Cameras().size(); ++i)
        {
            Simian::Camera* const cam = Simian::Game::Instance().RenderQueue().Cameras()[i];
            if (SIsFlagSet(cam->Layers(), layer_))
            {
                camera_ = cam;
                break;
            }
        }

        timer_ = flickerDuration_;
    }

    void GCLightningEffect::OnDeinit()
    {
        indexBuffer_->Unload();
        Simian::Game::Instance().GraphicsDevice().DestroyIndexBuffer(indexBuffer_);

        vertexBuffer_->Unload();
        Simian::Game::Instance().GraphicsDevice().DestroyVertexBuffer(vertexBuffer_);
    }

    void GCLightningEffect::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Generations", generations_);
        data.AddData("Deviation", deviation_);
        data.AddData("Thickness", thickness_);
        data.AddData("FlickerDuration", flickerDuration_);
        data.AddData("ForkChance", forkChance_);
        data.AddData("ForkScale", forkScale_);
        data.AddData("DeviationDecay", deviationDecay_);
        data.AddData("ColorDecay", colorDecay_);
        data.AddData("Visible", visible_);

        data.AddData("Layer", layer_ == Simian::RenderQueue::RL_PRESCENE ? "PRESCENE" :
                              layer_ == Simian::RenderQueue::RL_SCENE ? "SCENE" :
                              layer_ == Simian::RenderQueue::RL_POSTSCENE ? "POSTSCENE" :
                              layer_ == Simian::RenderQueue::RL_PREUI ? "PREUI" :
                              layer_ == Simian::RenderQueue::RL_UI ? "UI" :
                              layer_ == Simian::RenderQueue::RL_POSTUI ? "POSTUI" :
                              "SCENE");

        data.AddData("MaterialFile", materialFile_);

        Simian::FileObjectSubNode* origin = data.AddObject("Origin");
        origin->AddData("X", origin_.X());
        origin->AddData("Y", origin_.Y());
        origin->AddData("Z", origin_.Z());

        Simian::FileObjectSubNode* target = data.AddObject("Target");
        target->AddData("X", target_.X());
        target->AddData("Y", target_.Y());
        target->AddData("Z", target_.Z());
    }

    void GCLightningEffect::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("Generations", generations_, generations_);
        data.Data("Deviation", deviation_, deviation_);
        data.Data("Thickness", thickness_, thickness_);
        data.Data("FlickerDuration", flickerDuration_, flickerDuration_);
        data.Data("ForkChance", forkChance_, forkChance_);
        data.Data("ForkScale", forkScale_, forkScale_);
        data.Data("DeviationDecay", deviationDecay_, deviationDecay_);
        data.Data("ColorDecay", colorDecay_, colorDecay_);
        data.Data("Visible", visible_, visible_);

        std::string layer;
        data.Data("Layer", layer, "SCENE");
        layer_ = static_cast<Simian::u8>(
                 layer == "PRESCENE" ? Simian::RenderQueue::RL_PRESCENE :
                 layer == "SCENE" ? Simian::RenderQueue::RL_SCENE :
                 layer == "POSTSCENE" ? Simian::RenderQueue::RL_POSTSCENE :
                 layer == "PREUI" ? Simian::RenderQueue::RL_PREUI :
                 layer == "UI" ? Simian::RenderQueue::RL_UI :
                 layer == "POSTUI" ? Simian::RenderQueue::RL_POSTUI :
                 Simian::RenderQueue::RL_SCENE);

        data.Data("MaterialFile", materialFile_, materialFile_);

        const Simian::FileObjectSubNode* origin = data.Object("Origin");
        if (origin)
        {
            float x, y, z;
            origin->Data("X", x, origin_.X());
            origin->Data("Y", y, origin_.Y());
            origin->Data("Z", z, origin_.Z());
            origin_ = Simian::Vector3(x, y, z);
        }

        const Simian::FileObjectSubNode* target = data.Object("Target");
        if (target)
        {
            float x, y, z;
            target->Data("X", x, target_.X());
            target->Data("Y", y, target_.Y());
            target->Data("Z", z, target_.Z());
            target_ = Simian::Vector3(x, y, z);
        }
    }
}
