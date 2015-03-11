#include "inputDeviceManagerW.h"
#include "inputDeviceFactory.h"
#include "keyboard.h"
#include "mouse.h"

#define BUFFERSIZE 256

namespace Util
{
    static unsigned int DATA_SIZE = BUFFERSIZE;
    static unsigned int MaxDeviceCount = 32;

    //--------------------------------------------------------------------------

    InputDeviceManager::InputDeviceManager()
        :   keyboard_(0),
            mouse_(0),
            devices_(0)
    {
        for(unsigned int i = 0; i < MaxDeviceCount; ++i)
            deviceIDPool_.Push(i);
    }

    InputDeviceManager::~InputDeviceManager()
    {
    }

    //--------------------------------------------------------------------------

    void InputDeviceManager::Load()
    {
        
    }

    void InputDeviceManager::Unload()
    {
        if(devices_)
        {
            for(unsigned int type = 0; type < ID_TOTAL; ++type)
                removeDevices(type);
            delete devices_;
            devices_ = 0;
        }
    }

    void InputDeviceManager::Update(HWND hwnd,
                                    UINT msg,
                                    WPARAM wParam,
                                    LPARAM lParam)
    {
        checkForNewDevices();

        for(DeviceList::iterator it = devices_->begin(); it != devices_->end(); ++it)
        {
            InputDevice::UpdateInfo info;
            info.hwnd_ = hwnd;
            info.lParam_ = lParam;
            info.msg_ = msg;
            info.wParam_ = wParam;

            (*it)->Update(info);
    }

    }

    std::vector<InputDevice*> InputDeviceManager::GetAllDevices(unsigned int type) const
    {
        std::vector<InputDevice*> ret;
        for(DeviceList::iterator it = devices_->begin(); it != devices_->end(); ++it)
        {
            if((*it)->Type() == type)
                ret.push_back(*it);
        }
        return ret;
    }

    //--------------------------------------------------------------------------

    void InputDeviceManager::removeDevice(InputDevice* device)
    {
        if(!device || !devices_)
            return;

        delete device;
        devices_->erase(device);
    }

    void InputDeviceManager::removeDevices(unsigned int type)
    {
        if(!devices_)
            return;

        for(DeviceList::iterator it = devices_->begin(); it != devices_->end(); ++it)
        {
            if((*it)->Type() == type)
            {
                DeviceList::iterator temp = it;
                ++temp;
                
                delete *it;
                devices_->erase(it);
                it = temp;
            }
        }
    }

    void InputDeviceManager::checkForNewDevices()
    {
        unsigned int deviceCount;
        PRAWINPUTDEVICELIST deviceList;

        GetRawInputDeviceList(NULL, &deviceCount, sizeof(RAWINPUTDEVICELIST));
        deviceList = reinterpret_cast<PRAWINPUTDEVICELIST>(malloc(sizeof(RAWINPUTDEVICELIST) * deviceCount));
        GetRawInputDeviceList(deviceList, &deviceCount, sizeof(RAWINPUTDEVICELIST));

        for(unsigned int i = 0; i < deviceCount; ++i)
        {
            DWORD type = deviceList[i].dwType; 
            if(type == RIM_TYPEKEYBOARD)
            {
                addDevice<Keyboard>(deviceList[i].hDevice,ID_KEYBOARD);
                continue;
            }
            else if(type == RIM_TYPEMOUSE)
            {
                addDevice<Mouse>(deviceList[i].hDevice,ID_MOUSE);
                continue;
            }

            if(type == RIM_TYPEHID)
            {


                //deviceList[i].
            }
        }
        free(deviceList);
    }

    //--------------------------------------------------------------------------

    bool InputDeviceManager::InputDeviceIDLessPred::operator()(InputDevice* a, 
                                                               InputDevice* b)
    {
        return a->Id() < b->Id();
    }
}
 