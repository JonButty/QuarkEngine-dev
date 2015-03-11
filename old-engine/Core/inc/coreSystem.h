#ifndef SYSTEM_H
#define SYSTEM_H

#include "singleton.h"
#include "vector2.h"
#include "os.h"
#include "inputDevice.h"

#include "configCoreDLL.h"

#include <string>

namespace GFX
{
    class Camera;
}

namespace Core
{
    class ENGINE_CORE_EXPORT System
        :   public Util::Singleton<System>
    {
    public:
        System();
        void Load();
        void Run();
        void Unload();
    private:
        float dt_;
        //Util::InputDeviceHandle keyboard_;
    };
}

#endif