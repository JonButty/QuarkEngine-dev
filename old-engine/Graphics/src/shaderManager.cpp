#include "shaderManager.h"

namespace GFX
{
    ShaderManager::ShaderManager()
    {
        for(unsigned int i = 0; i < ST_TOTAL; ++i)
            shaderPool_[i] = 0;
    }

    ShaderManager::~ShaderManager()
    {
    }

    void ShaderManager::Load()
    {
    }

    void ShaderManager::Unload()
    {
        for(unsigned int i = 0; i < ST_TOTAL; ++i)
        {
            if(shaderPool_[i])
                ShaderFactory::Instance().DestroyInstance(i,shaderPool_[i]);
        }
    }
}