/************************************************************************/
/*!
\file		TextureCache.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/TextureCache.h"
#include "Simian/Texture.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/LogManager.h"

namespace Simian
{
    static const u32 UNLOADED_CACHE_SIZE = 64;

    TextureCache::TextureResource::TextureResource( Texture* tex, const DataLocation& location, u32 levels )
        : TextureRes(tex),
          Location(location),
          Levels(levels)
    {
    }

    //--------------------------------------------------------------------------

    TextureCache::TextureCache( GraphicsDevice* device )
        : device_(device)
    {
        unloadedFileQueue_.reserve(UNLOADED_CACHE_SIZE);
        unloadedTextureQueue_.reserve(UNLOADED_CACHE_SIZE);
    }

    //--------------------------------------------------------------------------

    GraphicsDevice* TextureCache::Device() const
    {
        return device_;
    }

    void TextureCache::ErrorCallback( const Simian::Delegate& val )
    {
        errorCallback_ = val;
    }

    void TextureCache::ProgressCallback( const Simian::Delegate& val )
    {
        progressCallback_ = val;
    }

    bool TextureCache::Invalidated() const
    {
        return unloadedFileQueue_.size() || unloadedTextureQueue_.size();
    }

    //--------------------------------------------------------------------------

    Texture* TextureCache::createInstance_( const DataLocation& key, const u32& levels )
    {
        Texture* texture;
        device_->CreateTexture(texture);
        unloadedFileQueue_.push_back(TextureResource(texture, key, levels));
        return texture;
    }
    

    void TextureCache::unloadAll_()
    {
        // unload everything that has been loaded and delete the rest.
        for (u32 i = 0; i < loaded_.size(); ++i)
            loaded_[i].Texture->Unload();
        loaded_.clear();
        unloadedFileQueue_.clear();
        unloadedTextureQueue_.clear();
    }

    void TextureCache::destroyInstance_( Texture* instance )
    {
        device_->DestroyTexture(instance);
    }

    //--------------------------------------------------------------------------

    Texture* TextureCache::GetImmediate( const DataLocation& path, const u32& levels )
    {
        std::map<std::string, Texture*>::iterator i = cache_.find(path.Identifier());
        if (i == cache_.end())
        {
            Texture* texture;
            device_->CreateTexture(texture);
            cache_[path.Identifier()] = texture;

            TextureResource resource(texture, path, levels);
            if (!texture->LoadImageFile(path))
            {
                ErrorCallbackParam param;
                param.Handled = false;
                param.Phase = ErrorCallbackParam::LP_FILE;
                param.TextureRes = &resource;

                errorCallback_(param);

                if (!param.Handled)
                {
                    SWarn(std::string("Unhandled texture load image failure. Location: ") + path.Identifier());
                    return texture;
                }
            }

            if (!texture->LoadTexture(levels))
            {
                ErrorCallbackParam param;
                param.Handled = false;
                param.Phase = ErrorCallbackParam::LP_TEXTURE;
                param.TextureRes = &resource;

                errorCallback_(param);

                if (!param.Handled)
                {
                    SWarn(std::string("Unhandled texture load texture failure. Location: ") + path.Identifier());
                    texture->Unload();
                    return texture;
                }
            }

            return texture;
        }
        return i->second;
    }

    void TextureCache::LoadFiles()
    {
        for (u32 i = 0; i < unloadedFileQueue_.size(); ++i)
        {
            bool success = unloadedFileQueue_[i].TextureRes->LoadImageFile(unloadedFileQueue_[i].Location);

            if (!success)
            {
                // we failed.. see if we want to handle this
                ErrorCallbackParam param;
                param.Handled = false;
                param.Phase = ErrorCallbackParam::LP_FILE;
                param.TextureRes = &unloadedFileQueue_[i];
                errorCallback_(param);

                // error was not handled, don't continue processing this
                if (!param.Handled)
                {
                    SWarn(std::string("Unhandled texture load image failure. Location: ") + unloadedFileQueue_[i].Location.Identifier());
                    continue;
                }
            }

            unloadedTextureQueue_.push_back(unloadedFileQueue_[i]);
        }
        unloadedFileQueue_.clear();
    }

    void TextureCache::LoadTextures()
    {
        for (u32 i = 0; i < unloadedTextureQueue_.size(); ++i)
        {
            bool success = unloadedTextureQueue_[i].TextureRes->LoadTexture(unloadedTextureQueue_[i].Levels);

            if (!success)
            {
                ErrorCallbackParam param;
                param.Handled = false;
                param.Phase = ErrorCallbackParam::LP_TEXTURE;
                param.TextureRes = &unloadedTextureQueue_[i];

                errorCallback_(param);

                if (!param.Handled)
                {
                    // not handled.. unload this texture and toss it out
                    SWarn(std::string("Unhandled texture load texture failure. Location: ") + unloadedFileQueue_[i].Location.Identifier());
                    unloadedTextureQueue_[i].TextureRes->Unload();
                    continue;
                }
            }

            // call the progress callback
            progressCallback_();

            LoadedTexture texture = { unloadedTextureQueue_[i].TextureRes, unloadedTextureQueue_[i].Location.Identifier() };
            loaded_.push_back(texture);
        }
        unloadedTextureQueue_.clear();
    }

    void TextureCache::Reload()
    {
        // unload all loaded and add them to the unloaded file queue
        for (u32 i = 0; i < loaded_.size(); ++i)
        {
            loaded_[i].Texture->Unload();
            unloadedFileQueue_.push_back(TextureResource(loaded_[i].Texture, 
                                                         loaded_[i].Location, 
                                                         loaded_[i].Levels));
        }
        loaded_.clear();

        // process unloaded queues.
        LoadFiles();
        LoadTextures();
    }
}
