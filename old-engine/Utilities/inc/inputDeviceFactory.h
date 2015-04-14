#ifndef INPUTDEVICEFACTORY_H
#define INPUTDEVICEFACTORY_H

#include "factory.h"
#include "singleton.h"
#include "inputDevice.h"
#include "inputDeviceTypes.h"
#include "configUtilitiesDLL.h"

namespace Util
{
    class ENGINE_UTILITIES_EXPORT InputDeviceFactory
        :   public Factory<InputDevice, ID_TOTAL>,
            public Singleton<InputDeviceFactory>
    {
    };
}

#endif