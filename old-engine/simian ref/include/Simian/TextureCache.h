/************************************************************************/
/*!
\file		TextureCache.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_TEXURECACHE_H_
#define SIMIAN_TEXURECACHE_H_

#include "SimianPlatform.h"
#include "DataCache.h"
#include "Delegate.h"

#include <vector>

namespace Simian
{
    class Texture;
    class GraphicsDevice;

    class SIMIAN_EXPORT TextureCache: public DataCache<Texture, u32>
    {
    public:
        struct TextureResource
        {
            Texture* TextureRes;
            DataLocation Location;
            u32 Levels;

            TextureResource(Texture* tex, const DataLocation& location, u32 levels);
        };

        struct LoadedTexture
        {
            Simian::Texture* Texture;
            std::string Location;
            u32 Levels;
        };

        struct ErrorCallbackParam: public DelegateParameter
        {
            TextureResource* TextureRes;
            bool Handled;
            enum LoadPhase
            {
                LP_FILE,
                LP_TEXTURE
            } Phase;
        };
    private:
        GraphicsDevice* device_;
        std::vector<TextureResource> unloadedFileQueue_;
        std::vector<TextureResource> unloadedTextureQueue_;
        std::vector<LoadedTexture> loaded_;

        Delegate errorCallback_;
        Delegate progressCallback_;
    public:
        GraphicsDevice* Device() const;

        void ErrorCallback(const Simian::Delegate& val);
        void ProgressCallback(const Simian::Delegate& val);

        bool Invalidated() const;
    private:
        void unloadAll_();
        Texture* createInstance_(const DataLocation& key, const u32& levels);
        void destroyInstance_(Texture* instance);
    public:
        TextureCache(GraphicsDevice* device);

        Texture* GetImmediate(const DataLocation& path, const u32& levels = 0);

        void LoadFiles();
        void LoadTextures();

        void Reload();
    };
}

#endif
