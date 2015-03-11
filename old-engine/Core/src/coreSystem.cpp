//#include <vld.h>
#include "coreSystem.h"
#include "configGFX.h"
#include "inputDeviceManager.h"
#include "console.h"
#include "keyboard.h"
#include "gfxSystem.h"
#include "color.h"
#include "utilSystem.h"
#include "coreLogger.h"
#include "cameraManager.h"
#include "camera.h"
#include "os.h"

namespace Core
{
    System::System()
        :   dt_(0)
    {
    }

    void System::Load()
    {
        Util::System::Instance().Load();
        GFX::System::Instance().Load();
        CORE_LOG("All systems loaded");
        //keyboard_ = Util::InputDeviceManager::Instance().GetAllDeviceHandles(Util::ID_KEYBOARD)[0];
    }

    void System::Run()
    {
        MSG msg;
        bool done = false;

        // Initialize the message structure.
        ZeroMemory(&msg, sizeof(MSG));

        while(!done)// && !Util::InputDeviceManager::Instance().DeviceTriggered(keyboard_,Util::K_ESCAPE))
        {
            // Handle the windows messages.
            if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            // If windows signals to end the application then exit out.
            if(msg.message == WM_QUIT)
                done = true;

            // Replace with system component delegates
            float prevTick = Util::OS::Instance().GetOSTicks();
            GFX::System::Instance().Update(dt_);
            dt_ = Util::OS::Instance().GetOSTicks() - prevTick;
        }
    }

    void System::Unload()
    {
        GFX::System::Instance().Unload();
        Util::System::Instance().Unload();
    }
}