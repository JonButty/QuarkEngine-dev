#ifndef UTILSYSTEM_H
#define UTILSYSTEM_H

#include "configUtilitiesDLL.h"
#include "singleton.h"

namespace Util
{
    class ENGINE_UTILITIES_EXPORT System
        :   public Singleton<System>
    {
    public:
        System();
        ~System();
        void Load();
        void Unload();
    };
}

#endif