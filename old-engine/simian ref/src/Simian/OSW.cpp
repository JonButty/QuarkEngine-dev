/*****************************************************************************/
/*!
\file		OSW.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/SimianPlatform.h"
#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32

#include "Simian/OSW.h"
#include "Simian/Utility.h"
#include "Simian/Delegate.h"

#include <string>
#include <vector>
#include <crtdbg.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShlObj.h>
#include <direct.h>

namespace Simian
{
    static std::vector<Delegate> osHooks_;
    
    struct OSHookParameter: public DelegateParameter
    {
        UINT Msg;
        LPARAM LParam;
        WPARAM WParam;
        bool Handled;
    };

    // pimpl of win32 windowhandle
    struct WWindowHandle
    {
        HWND hWnd;
        s32 x, y, width, height;
        OS::WindowStyle style;
        bool closed;
        std::string title;

        WWindowHandle(): hWnd(0), x(0), y(0), width(0), height(0), closed(false) {}
    };

    void OS::Assert(bool expr, const std::wstring& msg) const
    {
        SUNREFERENCED_PARAMETER(expr);
        SUNREFERENCED_PARAMETER(msg);
        _ASSERT_EXPR(expr, msg.c_str());
    }

    std::string OS::GetSpecialFolderPath(SpecialFolder folder) const
    {
        s8 path[MAX_PATH];
        s32 folderid = CSIDL_MYDOCUMENTS;
        if (folder == OSBase::SF_DOCUMENTS)
            folderid = CSIDL_MYDOCUMENTS;
        else if (folder == OSBase::SF_USER)
            folderid = CSIDL_PROFILE;
        SHGetFolderPath(NULL, folderid, NULL, 0, path);
        return std::string(path);
    }

    void OS::GetFilesInDirectory( const std::string& path, std::vector<std::string>& files ) const
    {
        std::string fullPath = path + "\\*";
        WIN32_FIND_DATA findData;
        HANDLE findHandle = FindFirstFile(fullPath.c_str(), &findData);
        if (findHandle == INVALID_HANDLE_VALUE)
            return; // no files found
        do 
        {
            files.push_back(findData.cFileName);
        } while (FindNextFile(findHandle, &findData));
        FindClose(findHandle);
    }

    void registerOSHook_(const Delegate& delegate)
    {
        osHooks_.push_back(delegate);
    }

    static LRESULT CALLBACK wndProc_(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        WWindowHandle& handle = *reinterpret_cast<WWindowHandle*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        switch (msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            handle.closed = true;
            return 0;
        }

        // pimpl os hooks
        for (u32 i = 0; i < osHooks_.size(); ++i)
        {
            // set up parameter
            OSHookParameter param;
            param.Msg = msg;
            param.LParam = lParam;
            param.WParam = wParam;
            param.Handled = false;

            // call the delegate
            osHooks_[i](param);

            // if handled, return out!
            if (param.Handled)
                return 0;
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    bool OS::WindowCreate( WindowHandle& window, const Simian::Vector2& position, const Simian::Vector2& size, const std::string& title, WindowStyle wstyle, u32 icon ) const
    {
        WWindowHandle* handle = new WWindowHandle;

        // create the window
        WNDCLASS wndClass;
        ZeroMemory(&wndClass, sizeof(WNDCLASS));

        wndClass.style = CS_OWNDC;
        wndClass.cbWndExtra = sizeof(WWindowHandle*);
        wndClass.lpfnWndProc = wndProc_;
        wndClass.hInstance = GetModuleHandle(NULL);
        wndClass.hIcon = LoadIcon(!icon ? NULL : GetModuleHandle(NULL), 
                                  !icon ? IDI_WINLOGO : MAKEINTRESOURCE(icon));
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        wndClass.lpszClassName = title.c_str();

        if (!RegisterClass(&wndClass))
        {
            delete handle;
            return false;
        }

        DWORD exStyle = 0, style = 0;
        handle->x = static_cast<int>(position.X());
        handle->y = static_cast<int>(position.Y());
        handle->width = static_cast<int>(size.X());
        handle->height = static_cast<int>(size.Y());
        handle->style = wstyle;
        handle->title = title;

        s32 x = handle->x, y = handle->y, width = handle->width, height = handle->height;
        if (wstyle == SWS_BORDERS)
        {
            exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
            style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

            RECT windowRect = { 0, 0, static_cast<LONG>(handle->width), static_cast<LONG>(handle->height) };
            AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);
            x = (GetSystemMetrics(SM_CXFULLSCREEN) - (windowRect.right - windowRect.left))/2;
            y = (GetSystemMetrics(SM_CYFULLSCREEN) - (windowRect.bottom - windowRect.top))/2;
            width = windowRect.right - windowRect.left;
            height = windowRect.bottom - windowRect.top;
        }
        else
        {
            exStyle = WS_EX_TOPMOST;
            style = WS_POPUP | WS_SYSMENU;
        }

        handle->hWnd = CreateWindowEx(exStyle, title.c_str(), title.c_str(), 
            style, x, y, width, height, 
            NULL, NULL, NULL, NULL);
                        
        if (!handle->hWnd)
        {
            UnregisterClass(title.c_str(), GetModuleHandle(NULL));
            delete handle;
            return false;
        }

        SetWindowLongPtr(handle->hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(handle));

        UpdateWindow(handle->hWnd);
        ShowWindow(handle->hWnd, SW_SHOW);
        SetForegroundWindow(handle->hWnd);
        SetFocus(handle->hWnd);

        window = handle;

        return true;
    }

    bool OS::WindowUpdate( WindowHandle& window ) const
    {
        WWindowHandle& handle = *reinterpret_cast<WWindowHandle*>(window);

        MSG msg;

        // do message pump here
        if (PeekMessage(&msg, handle.hWnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            return false;
        }

        return true;
    }

    void OS::WindowDestroy( WindowHandle& window ) const
    {
        WWindowHandle& handle = *reinterpret_cast<WWindowHandle*>(window);

        if (handle.hWnd)
        {
            DestroyWindow(handle.hWnd);
            UnregisterClass(handle.title.c_str(), GetModuleHandle(NULL));
        }
        
        delete &handle;
        window = 0;
    }

    void OS::WindowMove( WindowHandle& window, const Simian::Vector2& position ) const
    {
        WWindowHandle& handle = *reinterpret_cast<WWindowHandle*>(window);

        handle.x = static_cast<s32>(position.X());
        handle.y = static_cast<s32>(position.Y());

        // makes sure the top most style works.
        SetWindowPos(handle.hWnd, handle.style == SWS_POPUP ? HWND_TOPMOST : HWND_NOTOPMOST, 
            handle.x, handle.y, handle.width, handle.height, 0);
    }

    void OS::WindowResize( WindowHandle& window, const Simian::Vector2& size ) const
    {
        WWindowHandle& handle = *reinterpret_cast<WWindowHandle*>(window);

        handle.width = static_cast<s32>(size.X());
        handle.height = static_cast<s32>(size.Y());

        // makes sure the top most style works.
        SetWindowPos(handle.hWnd, handle.style == SWS_POPUP ? HWND_TOPMOST : HWND_NOTOPMOST, 
            handle.x, handle.y, handle.width, handle.height, 0);
    }

    void OS::WindowRestyle( WindowHandle& window, WindowStyle wstyle ) const
    {
        WWindowHandle& handle = *reinterpret_cast<WWindowHandle*>(window);
        handle.style = wstyle;

        DWORD exStyle = 0, style = 0;

        s32 x = handle.x, y = handle.y, width = handle.width, height = handle.height;
        if (wstyle == SWS_BORDERS)
        {
            exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
            style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

            RECT windowRect = { 0, 0, static_cast<LONG>(handle.width), static_cast<LONG>(handle.height) };
            AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);
            x = (GetSystemMetrics(SM_CXFULLSCREEN) - (windowRect.right - windowRect.left))/2;
            y = (GetSystemMetrics(SM_CYFULLSCREEN) - (windowRect.bottom - windowRect.top))/2;
            width = windowRect.right - windowRect.left;
            height = windowRect.bottom - windowRect.top;
        }
        else
        {
            exStyle = WS_EX_TOPMOST;
            style = WS_POPUP | WS_SYSMENU;
            x = 0;
            y = 0;
        }

        SetWindowLongPtr(handle.hWnd, GWL_STYLE, style);
        SetWindowLongPtr(handle.hWnd, GWL_EXSTYLE, exStyle);
        UpdateWindow(handle.hWnd);
        ShowWindow(handle.hWnd, SW_SHOW);
        SetForegroundWindow(handle.hWnd);
        SetFocus(handle.hWnd);

        // makes sure the top most style works.
        SetWindowPos(handle.hWnd, handle.style == SWS_POPUP ? HWND_TOPMOST : HWND_NOTOPMOST, x, y, width, height, 0);
    }

    bool OS::WindowClosed( const WindowHandle& window ) const
    {
        const WWindowHandle& handle = *reinterpret_cast<const WWindowHandle*>(window);
        return handle.closed;
    }

    bool OS::WindowVisible( const WindowHandle& window ) const
    {
        const WWindowHandle& handle = *reinterpret_cast<const WWindowHandle*>(window);

        WINDOWINFO windowInfo;
        GetWindowInfo(handle.hWnd, &windowInfo);

        return windowInfo.dwWindowStatus == WS_ACTIVECAPTION && 
               ~windowInfo.dwStyle & WS_MINIMIZE;
    }

    void OS::CurrentDateTime( u8& day, u8& month, u16& year, u8& hour, u8& minute, u8& second ) const
    {
        SYSTEMTIME time;
        GetSystemTime(&time);
        day     = static_cast<u8>(time.wDay);
        month   = static_cast<u8>(time.wMonth);
        year    = time.wYear;
        hour    = static_cast<u8>(time.wHour);
        minute  = static_cast<u8>(time.wMinute);
        second  = static_cast<u8>(time.wSecond);
    }

    void OS::CurrentLocalDateTime( u8& day, u8& month, u16& year, u8& hour, u8& minute, u8& second ) const
    {
        SYSTEMTIME time;
        GetLocalTime(&time);
        day     = static_cast<u8>(time.wDay);
        month   = static_cast<u8>(time.wMonth);
        year    = time.wYear;
        hour    = static_cast<u8>(time.wHour);
        minute  = static_cast<u8>(time.wMinute);
        second  = static_cast<u8>(time.wSecond);
    }

    Simian::f32 OS::GetOSTicks() const
    {
        LARGE_INTEGER counter, freq;
        QueryPerformanceCounter(&counter);
        QueryPerformanceFrequency(&freq);

		// improves precision
        return static_cast<f32>(
			   static_cast<f64>(counter.QuadPart)/
			   static_cast<f64>(freq.QuadPart));
    }

    Simian::u64 OS::GetHighPerformanceCounter() const
    {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        return counter.QuadPart;
    }

    Simian::u64 OS::GetHighPerformanceFrequency() const
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return freq.QuadPart;
    }

    void OS::CursorSetVisibility( bool visible ) const
    {
        ShowCursor(visible ? TRUE : FALSE);
    }

    void OS::DesktopResolution( f32& width, f32& height ) const
    {
        width = static_cast<f32>(GetSystemMetrics(SM_CXSCREEN));
        height = static_cast<f32>(GetSystemMetrics(SM_CYSCREEN));
    }

    void OS::DirectoryCreate( const std::string& filePath ) const
    {
        std::string path = filePath;

        u32 pos = path.find_first_of('/');
        while (pos != std::string::npos)
        {
            path[pos] = '\\';
            pos = path.find_first_of('/');
        }

        _mkdir(path.c_str());
    }
}

extern "C"
{
    Simian::WindowHandle SIMIAN_EXPORT createWindowHandle_(HWND hwnd)
    {
        Simian::WWindowHandle* ret = new Simian::WWindowHandle();
        ret->hWnd = hwnd;
        return ret;
    }
};

#endif
