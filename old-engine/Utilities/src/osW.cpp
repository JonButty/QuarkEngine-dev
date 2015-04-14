#include "osW.h"
#include "inputDeviceManager.h"
#include "config.h"

#include <iostream>
#include <ShlObj.h>
#include <direct.h>

namespace Util
{
    static LRESULT CALLBACK wndProc_(HWND hwnd,
                                     UINT msg,
                                     WPARAM wParam,
                                     LPARAM lParam)
    {
        WindowHandle& handle = *reinterpret_cast<WindowHandle*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        switch (msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            InputDeviceManager::Instance().Update(hwnd,msg,wParam,lParam);
        }
        /*
        for (unsigned int i = 0; i < osHooks_.size(); ++i)
        {
            OSHookParameter param;
            param.Msg = msg;
            param.LParam = lParam;
            param.WParam = wParam;
            param.Handled = false;

            osHooks_[i](param);

            if (param.Handled)
                return 0;
        }
        */
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    //--------------------------------------------------------------------------
    
    float OS::GetOSTicks() const
    {
        LARGE_INTEGER counter, freq;
        QueryPerformanceCounter(&counter);
        QueryPerformanceFrequency(&freq);

        // improves precision
        return static_cast<float>(static_cast<double>(counter.QuadPart)/
                                  static_cast<double>(freq.QuadPart));
    }

    void OS::Load()
    {
        WNDCLASSEX wc;
        int posX, posY;

        // Get the instance of this application.
        hInst_ = GetModuleHandle(NULL);

        // Give the application a name.
        appName_ = ENGINE_NAME_W;

        // Setup the windows class with default settings.
        wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW | CS_PARENTDC;
        wc.lpfnWndProc   = wndProc_;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = hInst_;
        wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
        wc.hIconSm       = wc.hIcon;
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = appName_;
        wc.cbSize        = sizeof(WNDCLASSEX);

        // Register the window class.
        RegisterClassEx(&wc);

        // Determine the resolution of the clients desktop screen.
        screenWidth_  = GetSystemMetrics(SM_CXSCREEN);
        screenHeight_ = GetSystemMetrics(SM_CYSCREEN);

        unsigned long wstyle;

        // Setup the screen settings depending on whether it is running in full screen or in windowed mode.
#if ENGINE_GFX_FULLSCREENENGINE_MATH == ENGINE_MATH_SSE
        
            // If full screen set the screen to maximum size of the users desktop and 32bit.
            memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
            dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
            dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth_;
            dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight_;
            dmScreenSettings.dmBitsPerPel = 32;			
            dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

            // Change the display settings to full screen.
            ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

            // Set the position of the window to the top left corner.
            posX = posY = 0;
            wstyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
#else
            // If windowed then set it to 800x600 resolution.
            screenWidth_  = ENGINE_GFX_WIDTH;
            screenHeight_ = ENGINE_GFX_HEIGHT;

            // Place the window in the middle of the screen.
            posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth_)  / 2;
            posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight_) / 2;
            wstyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW;
#endif

        // Create the window with the screen settings and get the handle to it.
        handle_ = CreateWindowEx(WS_EX_APPWINDOW, 
                                 appName_, 
                                 appName_, 
                                 wstyle,
                                 posX,
                                 posY,
                                 screenWidth_,
                                 screenHeight_,
                                 NULL,
                                 NULL,
                                 hInst_,
                                 NULL);

        // Bring the window up on the screen and set it as main focus.
        ShowWindow(handle_, SW_SHOW);
        SetForegroundWindow(handle_);
        SetFocus(handle_);

        // Hide the mouse cursor.
#if ENGINE_GFX_CURSOR
        ShowCursor(true);
#else
        ShowCursor(false);
#endif
    }

    void OS::Unload()
    {
        // Show the mouse cursor.
        ShowCursor(true);

        // Fix the display settings if leaving full screen mode.
#if ENGINE_GFX_FULLSCREENENGINE_MATH == ENGINE_MATH_SSE
            ChangeDisplaySettings(NULL, 0);
#endif

        // Remove the window.
        //DestroyWindow(handle_);
        handle_ = NULL;

        // Remove the application instance.
        UnregisterClass(appName_, hInst_);
        hInst_ = NULL;
    }

    //--------------------------------------------------------------------------

    int OS::ScreenWidth() const
    {
        return screenWidth_;
    }

    int OS::ScreenHeight() const
    {
        return screenHeight_;
    }

    WindowHandle OS::Handle() const
    {
        return handle_;
    }
}