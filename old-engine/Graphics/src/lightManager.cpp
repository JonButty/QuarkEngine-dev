#include "lightManager.h"
#include "light.h"
#include "lightFactory.h"

#define MAX_LIGHT_COUNT 120

namespace GFX
{
    LightManager::LightManager()
    {
    }

    void LightManager::Load()
    {
        // Initialize handle pool
        for(unsigned int i = 0; i < MAX_LIGHT_COUNT; ++i)
            handlePool_.Push(i);
    }

    void LightManager::Unload()
    {
        for(unsigned int i = 0; i < LT_TOTAL; ++i)
        {
            std::map<LightHandle,Light*>::iterator it = lights_[i].begin();
            for(; it != lights_[i].end(); ++it)
            {
                handlePool_.Return(it->first);
                LightFactory::Instance().DestroyInstance(i,it->second);
            }
            lights_[i].clear();
        }
    }

    std::vector<LightHandle> LightManager::GetLights( unsigned int type ) const
    {
        std::vector<LightHandle> ret;

        std::map<LightHandle,Light*>::const_iterator it = lights_[type].begin();
        for(; it != lights_[type].end(); ++it)
            ret.push_back(it->first);
        return ret;
    }

    Light* LightManager::GetLight( LightHandle handle ) const
    {
        std::vector<LightHandle> ret;
        for(unsigned int i = 0; i < LT_TOTAL; ++i)
        {
            std::map<LightHandle,Light*>::const_iterator it = lights_[i].begin();
            for(; it != lights_[i].end(); ++it)
            {
                if(it->first == handle)
                    return it->second;
            }
        }
        return NULL;
    }

    GFX::LightHandle LightManager::AddLight( Light* light )
    {        
        LightHandle handle = handlePool_.Pop();
        lights_[light->Type()].insert(std::pair<LightHandle,Light*>(handle,light));
        return handle;
    }
}