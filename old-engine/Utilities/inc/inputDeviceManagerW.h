#ifndef SINGLEINPUTDEVICEMANAGERW_H
#define SINGLEINPUTDEVICEMANAGERW_H

#include "singleton.h"
#include "configUtilitiesDLL.h"
#include "inputDeviceTypes.h"
#include "inputDevice.h"
#include "pool.h"

#include <set>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Util
{
    // Using handles to reference devices is safer than using pointers
    // GetDevice() should be used when a function needs to call custom functions
    // declared in an inherited input device class
    class ENGINE_UTILITIES_EXPORT InputDeviceManager
        :   public Singleton<InputDeviceManager>
    {
    public:
        InputDeviceManager();
        ~InputDeviceManager();
        void Load();
        void Unload();
        void Update(HWND hwnd,
                    UINT msg,
                    WPARAM wParam,
                    LPARAM lParam);

        std::vector<InputDevice*> GetAllDevices(unsigned int type) const;
    private:
        template<typename T>
        void addDevice(HANDLE rawDevice,
                       unsigned int type)
        {
            if(!devices_)
                devices_ = new DeviceList;

            UINT size = 1024;
            CHAR tBuffer[1024] = {0};
            InputDevice* inputDevice = 0;
            RID_DEVICE_INFO rdi;
            UINT cbSize = sizeof(RID_DEVICE_INFO);

            if(GetRawInputDeviceInfo(rawDevice, RIDI_DEVICEINFO, &rdi, &cbSize) >= 0)
            {
                if(type == ID_MOUSE)
                    inputDevice = new T(static_cast<unsigned int>(rdi.mouse.dwNumberOfButtons));
                else if(type == ID_KEYBOARD)
                    inputDevice = new T(static_cast<unsigned int>(rdi.keyboard.dwNumberOfKeysTotal));

                inputDevice->Id(deviceIDPool_.Pop());
            }

            if(GetRawInputDeviceInfo(rawDevice, RIDI_DEVICENAME, tBuffer, &size) >= 0)
                inputDevice->Name(tBuffer);

            devices_->insert(inputDevice);
        }

        void removeDevice(InputDevice* device);
        void removeDevices(unsigned int type);
        void checkForNewDevices();
    public:
        struct InputDeviceIDLessPred
        {
            bool operator()(InputDevice* a,
                            InputDevice* b);
        };
    private:
        typedef std::multiset<InputDevice*,InputDeviceIDLessPred> DeviceList;
    private:
        InputDevice* keyboard_;
        InputDevice* mouse_;
        DeviceList* devices_;
        Pool<unsigned int> deviceIDPool_;
    };
}

#endif