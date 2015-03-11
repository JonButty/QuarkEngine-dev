/************************************************************************/
/*!
\file		MaterialTextureStage.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Material.h"
#include "Simian/Texture.h"
#include "Simian/RenderTexture.h"
#include "Simian/GraphicsDevice.h"

namespace Simian
{
    Material::TextureStage::TextureStage( Material* material )
        : material_(material),
          texture_(0),
          renderTexture_(0)
    {
    }

    Material::TextureStage::~TextureStage()
    {
    }

    //--------------------------------------------------------------------------

    Simian::Texture* Material::TextureStage::Texture() const
    {
        return texture_;
    }

    void Material::TextureStage::Texture( Simian::Texture* val )
    {
        texture_ = val;
    }

    Simian::RenderTexture** Material::TextureStage::RenderTexture() const
    { 
        return renderTexture_; 
    }

    void Material::TextureStage::RenderTexture(Simian::RenderTexture** val)
    { 
        renderTexture_ = val;
    }

    //--------------------------------------------------------------------------

    void Material::TextureStage::AddTextureStageState( u32 key, u32 value )
    {
        CurrentPrevPair cpPair;
        cpPair.Previous = 0;
        cpPair.Current = value;
        textureStates_.push_back(std::make_pair(key, cpPair));
    }

    void Material::TextureStage::Set( u32 sampler )
    {
        if (renderTexture_ && !*renderTexture_)
            return;

        TextureBase* texture = renderTexture_ ? 
            reinterpret_cast<TextureBase*>(*renderTexture_) : texture_;
        texture->Set(sampler);

        for (u32 i = 0; i < textureStates_.size(); ++i)
        {
            textureStates_[i].second.Previous = texture->TextureState(textureStates_[i].first);
            texture->TextureState(textureStates_[i].first, textureStates_[i].second.Current);
        }
    }

    void Material::TextureStage::Unset()
    {
        if (renderTexture_ && !*renderTexture_)
            return;

        TextureBase* texture = renderTexture_ ? 
            reinterpret_cast<TextureBase*>(*renderTexture_) : texture_;

        for (u32 i = 0; i < textureStates_.size(); ++i)
            texture->TextureState(textureStates_[i].first, textureStates_[i].second.Previous);

        texture->Unset();
    }

    void Material::TextureStage::Unload()
    {
        if (texture_ && texture_->Dynamic())
            material_->Device().DestroyTexture(texture_);
        texture_ = 0;
    }
}
