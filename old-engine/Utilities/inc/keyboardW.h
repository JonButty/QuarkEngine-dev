#ifndef KEYBOARDW_H
#define KEYBOARDW_H

#include "inputDevice.h"
#include "configUtilitiesDLL.h"
#include "factory.h"

namespace Util
{
    class ENGINE_UTILITIES_EXPORT Keyboard
        :   public InputDevice
    {
    public:
        Keyboard(unsigned int maxKeys);
        ~Keyboard();
    protected:
        void update_(const UpdateInfo& info);
        unsigned int getMappedKeyValue_(unsigned int val) const;
    };
}

#endif