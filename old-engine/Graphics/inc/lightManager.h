#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include "singleton.h"
#include "lightTypes.h"
#include "configGraphicsDLL.h"
#include "pool.h"

#include <map>
#include <vector>

namespace GFX
{
    class Light;

    typedef unsigned int LightHandle;

    class ENGINE_GRAPHICS_EXPORT LightManager
        :   public Util::Singleton<LightManager>
    {
    public:
        LightManager();
        void Load();
        void Unload();
        std::vector<LightHandle> GetLights(unsigned int type) const;
        Light* GetLight(LightHandle handle) const;
        LightHandle AddLight(Light* light);
    private:
        std::map<LightHandle,Light*> lights_[LT_TOTAL];
        Util::Pool<LightHandle> handlePool_;
    };
}

#endif