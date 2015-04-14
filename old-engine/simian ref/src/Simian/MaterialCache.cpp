/************************************************************************/
/*!
\file		MaterialCache.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/MaterialCache.h"
#include "Simian/Material.h"
#include "Simian/DataFileIO.h"
#include "Simian/TextureCache.h"
#include "Simian/ShaderCache.h"

namespace Simian
{
    MaterialCache::MaterialCache( GraphicsDevice* device, 
                                  Simian::TextureCache* cache, 
                                  Simian::ShaderCache* shaderCache )
        : device_(device),
          textureCache_(cache),
          shaderCache_(shaderCache),
          materialCount_(0)
    {
    }

    //--------------------------------------------------------------------------

    Simian::TextureCache* MaterialCache::TextureCache() const
    {
        return textureCache_;
    }

    void MaterialCache::TextureCache( Simian::TextureCache* val )
    {
        textureCache_ = val;
    }

    Simian::ShaderCache* MaterialCache::ShaderCache() const
    {
        return shaderCache_;
    }

    void MaterialCache::ShaderCache( Simian::ShaderCache* val )
    {
        shaderCache_ = val;
    }

    //--------------------------------------------------------------------------

    Material* MaterialCache::createInstance_( const DataLocation& location, void* const& )
    {
        Material* material = new Material(device_, textureCache_, shaderCache_, materialCount_++);
        material->Load<DataFileIODefault>(location);
        LoadedMaterial loadedMaterial = { material, location.Identifier() };
        loaded_.push_back(loadedMaterial);
        return material;
    }

    void MaterialCache::unloadAll_()
    {
        loaded_.clear();
        materialCount_ = 0;
    }

    void MaterialCache::destroyInstance_( Material* instance )
    {
        instance->Unload();
        delete instance;
    }

    //--------------------------------------------------------------------------

    void MaterialCache::Reload()
    {
        for (u32 i = 0; i < loaded_.size(); ++i)
        {
            loaded_[i].Material->Unload();
            loaded_[i].Material->Load<DataFileIODefault>(loaded_[i].Location);
        }

        // load all textures and shaders
        textureCache_->Reload();
        shaderCache_->Reload();
    }
}
