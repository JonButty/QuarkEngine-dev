#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include "singleton.h"
#include "shaderTypes.h"
#include "shaderFactory.h"
#include "configGraphicsDLL.h"
#include "gfxAPI.h"

namespace GFX
{
    class Shader;

    class ENGINE_GRAPHICS_EXPORT ShaderManager
        :   public Util::Singleton<ShaderManager>
    {
    public: 
        ShaderManager();
        ~ShaderManager();
        void Load();
        void Unload();
        
        template <typename T>
        void GetShader(unsigned int type,
                       T*& shader) 
        {
            if(!shaderPool_[type])
            {
                ShaderFactory::Instance().CreateInstance(type,shader);
                shaderPool_[type] = shader;
                shader->Load();
            }
            shader = dynamic_cast<T*>(shaderPool_[type]);
        }
    private:
        Shader* shaderPool_[ST_TOTAL];
    };
}

#endif