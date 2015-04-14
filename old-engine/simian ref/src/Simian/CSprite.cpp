/************************************************************************/
/*!
\file		CSprite.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CSprite.h"
#include "Simian/EngineComponents.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/VertexBuffer.h"
#include "Simian/MaterialCache.h"
#include "Simian/EnginePhases.h"
#include "Simian/RenderQueue.h"
#include "Simian/CTransform.h"
#include "Simian/Material.h"
#include "Simian/Texture.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CSprite> CSprite::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_SPRITE);

    RenderQueue* CSprite::renderQueue_;
    GraphicsDevice* CSprite::graphicsDevice_;
    MaterialCache* CSprite::materialCache_;

    CSprite::CSprite()
        : transform_(0),
          vertexBuffer_(0),
          material_(0),
          layer_(Simian::RenderQueue::RL_SCENE),
          visible_(true)
    {
    }

    //--------------------------------------------------------------------------

    Simian::RenderQueue* CSprite::RenderQueue()
    {
        return renderQueue_;
    }

    void CSprite::RenderQueue( Simian::RenderQueue* val )
    {
        renderQueue_ = val;
    }

    Simian::GraphicsDevice* CSprite::GraphicsDevice()
    {
        return graphicsDevice_;
    }

    void CSprite::GraphicsDevice( Simian::GraphicsDevice* val )
    {
        graphicsDevice_ = val;
    }

    Simian::MaterialCache* CSprite::MaterialCache()
    {
        return materialCache_;
    }

    void CSprite::MaterialCache( Simian::MaterialCache* val )
    {
        materialCache_ = val;
    }

    const Simian::Vector2& CSprite::Size() const
    {
        return size_;
    }

    void CSprite::Size( const Simian::Vector2& val )
    {
        size_ = val;
    }

    const Simian::Color& CSprite::Color() const
    {
        return color_;
    }

    void CSprite::Color( const Simian::Color& val )
    {
        color_ = val;
    }

    Simian::Material* CSprite::Material() const
    {
        return material_;
    }

    void CSprite::Material( Simian::Material* val )
    {
        material_ = val;
    }

    bool CSprite::Visible() const
    {
        return visible_;
    }

    void CSprite::Visible( bool val )
    {
        visible_ = val;
    }

    //--------------------------------------------------------------------------

    void CSprite::render_( DelegateParameter& )
    {
        if (!visible_)
            return;

        renderQueue_->AddRenderObject(RenderObject(*material_, layer_,
            transform_->World() * Simian::Matrix::Scale(size_.X(), size_.Y(), 1.0f),
            Delegate::CreateDelegate<CSprite, &CSprite::draw_>(this), 0));
    }

    void CSprite::draw_( DelegateParameter& )
    {
        const std::vector<Simian::Material::TextureStage>& textureStages = renderQueue_->ActivePass()->TextureStages();
        for (u32 i = 0; i < textureStages.size(); ++i)
        {
            oldStates_.push_back(textureStages[i].Texture()->TextureState(Texture::TSF_CONSTANT));
            textureStages[i].Texture()->TextureState(Texture::TSF_CONSTANT, color_.ColorU32());
        }
        graphicsDevice_->Draw(vertexBuffer_, GraphicsDevice::PT_TRIANGLESTRIP, 2);
        for (u32 i = 0; i < textureStages.size(); ++i)
            textureStages[i].Texture()->TextureState(Texture::TSF_CONSTANT, oldStates_[i]);
        oldStates_.clear();
    }

    //--------------------------------------------------------------------------

    void CSprite::OnSharedLoad()
    {
        // load material
        material_ = materialCache_->Load(materialLocation_);

        // vertex buffer
        graphicsDevice_->CreateVertexBuffer(vertexBuffer_);
        vertexBuffer_->Load(Simian::PositionColorUVVertex::Format, 4);
        Simian::PositionColorUVVertex vertices[4] = 
        {
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, -0.5f, 0.0f), Simian::Color(), Simian::Vector2(0, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, -0.5f, 0.0f), Simian::Color(), Simian::Vector2(1, 1)),
            Simian::PositionColorUVVertex(Simian::Vector3(-0.5f, 0.5f, 0.0f), Simian::Color(), Simian::Vector2(0, 0)),
            Simian::PositionColorUVVertex(Simian::Vector3(0.5f, 0.5f, 0.0f), Simian::Color(), Simian::Vector2(1, 0)),
        };
        vertexBuffer_->Data(vertices);

        // register callbacks
        RegisterCallback(P_RENDER, Delegate::CreateDelegate<CSprite, &CSprite::render_>(this));
    }

    void CSprite::OnSharedUnload()
    {
        vertexBuffer_->Unload();
        graphicsDevice_->DestroyVertexBuffer(vertexBuffer_);
    }

    void CSprite::OnAwake()
    {
        ComponentByType(C_TRANSFORM, transform_);
    }

    void CSprite::Serialize( FileObjectSubNode& data )
    {
        data.AddData("MaterialFile", materialLocation_);
        data.AddData("Visible", visible_);
        
        Simian::FileObjectSubNode* size = data.AddObject("Size");
        size->AddData("X", size_.X());
        size->AddData("Y", size_.Y());

        Simian::FileObjectSubNode* color = data.AddObject("Color");
        color->AddData("R", color_.R());
        color->AddData("G", color_.G());
        color->AddData("B", color_.B());
        color->AddData("A", color_.A());

        data.AddData("Layer", layer_ == Simian::RenderQueue::RL_PRESCENE ? "PRESCENE" : 
                              layer_ == Simian::RenderQueue::RL_SCENE ? "SCENE" : 
                              layer_ == Simian::RenderQueue::RL_POSTSCENE ? "POSTSCENE" : 
                              layer_ == Simian::RenderQueue::RL_PREUI ? "PREUI" : 
                              layer_ == Simian::RenderQueue::RL_UI ? "UI" : 
                              layer_ == Simian::RenderQueue::RL_POSTUI ? "POSTUI" : 
                              "SCENE");
    }

    void CSprite::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("MaterialFile", materialLocation_, materialLocation_);
        data.Data("Visible", visible_, visible_);

        std::string layer;
        data.Data("Layer", layer, "");
        layer_ = static_cast<u8>(
                 layer == "PRESCENE" ? Simian::RenderQueue::RL_PRESCENE :
                 layer == "SCENE" ? Simian::RenderQueue::RL_SCENE :
                 layer == "POSTSCENE" ? Simian::RenderQueue::RL_POSTSCENE :
                 layer == "PREUI" ? Simian::RenderQueue::RL_PREUI :
                 layer == "UI" ? Simian::RenderQueue::RL_UI :
                 layer == "POSTUI" ? Simian::RenderQueue::RL_POSTUI :
                 layer_);

        const Simian::FileObjectSubNode* size = data.Object("Size");
        if (size)
        {
            float x, y;
            size->Data("X", x, size_.X());
            size->Data("Y", y, size_.Y());
            size_ = Simian::Vector2(x, y);
        }

        const Simian::FileObjectSubNode* color = data.Object("Color");
        if (color)
        {
            float r, g, b, a;
            color->Data("R", r, color_.R());
            color->Data("G", g, color_.G());
            color->Data("B", b, color_.B());
            color->Data("A", a, color_.A());
            color_ = Simian::Color(r, g, b, a);
        }
    }
}
