/************************************************************************/
/*!
\file		MaterialPass.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Material.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/Texture.h"
#include "Simian/Shader.h"

namespace Simian
{
    Material::Pass::Pass( Material* parent )
        : parent_(parent),
          shader_(0),
          shaderCloned_(false)
    {
    }

    Material::Pass::~Pass()
    {
    }

    //--------------------------------------------------------------------------

    const std::vector<Material::TextureStage>& Material::Pass::TextureStages() const
    {
        return textureStages_;
    }

    Simian::Shader* Material::Pass::Shader() const
    {
        return shader_;
    }

    //--------------------------------------------------------------------------

    void Material::Pass::Set()
    {
        // set render states
        for (u32 i = 0; i < renderStates_.size(); ++i)
        {
            renderStates_[i].second.Previous = parent_->Device().RenderState(renderStates_[i].first);
            parent_->Device().RenderState(renderStates_[i].first, renderStates_[i].second.Current);
        }
        
        for (u32 i = 0; i < textureStages_.size(); ++i)
        {
            textureStages_[i].Set(i);
            for (u32 j = 0; j < samplerStates_.size(); ++j)
            {
                if (textureStages_[i].RenderTexture() && !*textureStages_[i].RenderTexture()) continue;
                TextureBase* texture = textureStages_[i].RenderTexture() ? 
                    reinterpret_cast<TextureBase*>(*textureStages_[i].RenderTexture()) : textureStages_[i].Texture();
                texture->SamplerState(samplerStates_[j].first, samplerStates_[j].second);
            }
        }

        if (shader_)
            shader_->Set();
    }

    void Material::Pass::Unset()
    {
        if (shader_)
            shader_->Unset();

        for (u32 i = 0; i < textureStages_.size(); ++i)
            textureStages_[i].Unset();

        for (u32 i = 0; i < renderStates_.size(); ++i)
            parent_->Device().RenderState(renderStates_[i].first, renderStates_[i].second.Previous);
    }

    void Material::Pass::AddSamplerState(u32 samplerState, u32 value)
    {
        samplerStates_.push_back(std::make_pair(samplerState, value));
    }

    void Material::Pass::AddRenderState( u32 renderState, u32 value )
    {
        CurrentPrevPair cppair;
        cppair.Previous = 0;
        cppair.Current = value;
        renderStates_.push_back(std::make_pair(renderState, cppair));
        parent_->alphaBlended_ = renderState == GraphicsDevice::RS_BLENDING && value ? 
            true : parent_->alphaBlended_;
    }

    void Material::Pass::AddTextureStage( const TextureStage& texture )
    {
        textureStages_.push_back(texture);
    }

    void Material::Pass::Unload()
    {
        // if the shader is a copy we must destroy it
        if (shaderCloned_)
        {
            shader_->Unload();
            delete shader_;
            shader_ = 0;
            shaderCloned_ = false;
        }

        for (u32 i = 0; i < textureStages_.size(); ++i)
            textureStages_[i].Unload();
        textureStages_.clear();
    }
}
