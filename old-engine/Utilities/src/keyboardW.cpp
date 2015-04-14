#include "keyboardW.h"
#include "inputDeviceFactory.h"
#include "inputDeviceTypes.h"

#include "WinUser.h"

namespace Util
{
    Keyboard::Keyboard(unsigned int maxKeys)
        :   InputDevice(ID_KEYBOARD,maxKeys)
    {
    }

    Keyboard::~Keyboard()
    {
    }

    void Keyboard::update_(const UpdateInfo&)
    {
    }

    unsigned int Keyboard::getMappedKeyValue_(unsigned int val) const
    {
        return val;
    }
}