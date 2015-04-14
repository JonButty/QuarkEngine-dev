/************************************************************************/
/*!
\file		ShaderCache.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/ShaderCache.h"
#include "Simian/Shader.h"
#include "Simian/DataFileIO.h"
#include "Simian/LogManager.h"

namespace Simian
{
    ShaderCache::ShaderResource::ShaderResource( Simian::Shader* shader, const DataLocation& data )
        : Shader(shader), Data(data)
    {
    }

    void ShaderCache::ShaderResource::Load( ShaderCache* cache )
    {
        // try to load the shader
        if (!Shader->Load<DataFileIODefault>(Data))
        {
            SWarn("Failed to load shader: " << Data.Identifier());
            return;
        }

        // add it to loaded queue only if it loaded right
        LoadedShader shader = { Shader, Data.Identifier() };
        cache->loaded_.push_back(shader);
    }

    //--------------------------------------------------------------------------

    ShaderCache::ShaderCache( GraphicsDevice* device )
        : device_(device)
    {
    }

    //--------------------------------------------------------------------------

    bool ShaderCache::Invalidated() const
    {
        return loadQueue_.size() > 0;
    }

    //--------------------------------------------------------------------------

    Shader* ShaderCache::createInstance_( const DataLocation& location, void* const& )
    {
        Shader* shader = new Shader(device_);
        ShaderResource resource(shader, location);
        loadQueue_.push_back(resource);
        return shader;
    }

    void ShaderCache::destroyInstance_( Shader* instance )
    {
        delete instance;
    }

    void ShaderCache::unloadAll_()
    {
        // unload anything that has already been loaded.
        for (u32 i = 0; i < loaded_.size(); ++i)
            loaded_[i].Shader->Unload();

        // clear out all queues.
        loadQueue_.clear();
        loaded_.clear();
    }

    //--------------------------------------------------------------------------

    Shader* ShaderCache::GetImmediate( const DataLocation& data )
    {
        std::map<std::string, Shader*>::iterator i = cache_.find(data.Identifier());

        if (i != cache_.end())
            return i->second;

        Shader* shader = new Shader(device_);
        if (!shader->Load<DataFileIODefault>(data))
        {
            SWarn("Failed to load shader: " << data.Identifier());

            delete shader;
            return NULL;
        }
        LoadedShader loadedShader = { shader, data.Identifier() };
        loaded_.push_back(loadedShader);
        cache_[data.Identifier()] = shader;

        return shader;
    }

    void ShaderCache::LoadShaders()
    {
        // load the load queue
        std::for_each(loadQueue_.begin(), loadQueue_.end(),
            std::bind2nd(std::mem_fun_ref(&ShaderResource::Load), this));

        // clear load queue
        loadQueue_.clear();
    }

    void ShaderCache::Reload()
    {
        for (u32 i = 0; i < loaded_.size(); ++i)
        {
            loaded_[i].Shader->Unload();
            loadQueue_.push_back(ShaderResource(loaded_[i].Shader, loaded_[i].Location));
        }
        loaded_.clear();

        LoadShaders();
    }
}
