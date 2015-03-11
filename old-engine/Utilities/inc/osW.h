#ifndef OSWINDOWS_H
#define OSWINDOWS_H

#include "singleton.h"
#include "vector2.h"
#include "configUtilitiesDLL.h"

#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Util
{
    typedef HWND WindowHandle;

    class ENGINE_UTILITIES_EXPORT OS
        :   public Util::Singleton<OS>
    {
    public:
        float GetOSTicks() const;
        void Load();
        void Unload();
    public:
        int ScreenWidth() const;
        int ScreenHeight() const;
        WindowHandle Handle() const;
    private:
        LPCWSTR appName_;
        HINSTANCE hInst_;
        WindowHandle handle_;
        int screenWidth_;
        int screenHeight_;
    };
}

#endif