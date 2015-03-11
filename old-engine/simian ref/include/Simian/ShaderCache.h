/************************************************************************/
/*!
\file		ShaderCache.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SHADERCACHE_H_
#define SIMIAN_SHADERCACHE_H_

#include "SimianPlatform.h"
#include "DataCache.h"

#include <vector>

namespace Simian
{
    class GraphicsDevice;
    class Shader;

    class SIMIAN_EXPORT ShaderCache: public DataCache<Shader>
    {
    private:
        struct ShaderResource
        {
            Simian::Shader* Shader;
            DataLocation Data;

            ShaderResource(Simian::Shader* shader, const DataLocation& data);
            void Load(ShaderCache* cache);
        };

        struct LoadedShader
        {
            Simian::Shader* Shader;
            std::string Location;
        };
    private:
        GraphicsDevice* device_;

        std::vector<ShaderResource> loadQueue_;
        std::vector<LoadedShader> loaded_;
    public:
        bool Invalidated() const;
    private:
        Shader* createInstance_(const DataLocation& location, void* const& params);

        void destroyInstance_(Shader* instance);
        void unloadAll_();
    public:
        ShaderCache(GraphicsDevice* device);

        Shader* GetImmediate(const DataLocation& data);

        void LoadShaders();
        void Reload();

        friend struct ShaderResource;
    };
}

#endif
