/************************************************************************/
/*!
\file		WMouse.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_WMOUSE_H_
#define SIMIAN_WMOUSE_H_

#include "Simian/SimianPlatform.h"
#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32

#include <windows.h>
#include "Simian/Mouse.h"

namespace Simian
{
    class SIMIAN_EXPORT Mouse: public MouseBase
    {
    public:
        enum Key
        {
            KEY_LBUTTON = VK_LBUTTON,
            KEY_RBUTTON = VK_RBUTTON,
            KEY_MBUTTON = VK_MBUTTON,
            TOTAL_KEYS
        };
    private:
	    DWORD numEvents_;
	    DWORD numEventsRead_;
	    INPUT_RECORD inputRec_[8];
		HWND winHand_;
    private:
        void mouseWheelUpdate_(DelegateParameter& param);
    public:
        bool Pressed(const u32 key);
        bool Depressed(const u32 key);
        bool Triggered(const u32 key);
        bool Released(const u32 key);
		int  MouseWheel();
        bool MouseMove();
	    Vector2 MouseScreenPosition();
		Vector2 MouseClientPosition();
        Vector2 MouseNormalizedPosition();
		void WinHand(WindowHandle wh);
    public:
        Mouse();

        void Update(f32 dt);
    };
}

#endif

#endif
