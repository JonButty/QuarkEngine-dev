#ifndef CONSOLEW_H
#define CONSOLEW_H

#include "configUtilitiesDLL.h"
#include "singleton.h"

namespace Util
{
    class ENGINE_UTILITIES_EXPORT Console 
        :   public Singleton<Console>
    {
    public:
        static void Show();
    };
}

#endif