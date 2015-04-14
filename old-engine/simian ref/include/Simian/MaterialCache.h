/************************************************************************/
/*!
\file		MaterialCache.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_MATERIALCACHE_H_
#define SIMIAN_MATERIALCACHE_H_

#include "SimianPlatform.h"
#include "DataCache.h"

#include <vector>

namespace Simian
{
    class Material;
    class GraphicsDevice;
    class TextureCache;
    class ShaderCache;

    class SIMIAN_EXPORT MaterialCache: public DataCache<Material>
    {
    private:
        struct LoadedMaterial
        {
            Simian::Material* Material;
            std::string Location;
        };
    private:
        GraphicsDevice* device_;
        TextureCache* textureCache_;
        ShaderCache* shaderCache_;
        u32 materialCount_;

        // loaded materials
        std::vector<LoadedMaterial> loaded_;
    public:
        Simian::TextureCache* TextureCache() const;
        void TextureCache(Simian::TextureCache* val);

        Simian::ShaderCache* ShaderCache() const;
        void ShaderCache(Simian::ShaderCache* val);
    private:
        Material* createInstance_( const DataLocation& location, void* const& params );
        void unloadAll_();
        void destroyInstance_( Material* instance );
    public:
        MaterialCache(GraphicsDevice* device, 
                      Simian::TextureCache* cache, 
                      Simian::ShaderCache* shaderCache);

        void Reload();
    };
}

#endif
