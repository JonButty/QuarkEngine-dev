/************************************************************************/
/*!
\file		Mouse.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_MOUSE_H_
#define SIMIAN_MOUSE_H_

#include <windows.h>
#include "Simian/InputDevice.h"
#include "Simian/TemplateFactory.h"

namespace Simian
{
    class Mouse;

    class SIMIAN_EXPORT MouseBase: public InputDevice
    {
    public:
        enum Key
        {
            KEY_LBUTTON,
			KEY_RBUTTON,
			KEY_MBUTTON,
            TOTAL_KEYS = 5
        };
    private:
        static FactoryPlant<InputDevice, Mouse> factoryPlant_;
        typedef bool KeyMap[TOTAL_KEYS];
    protected:
        bool keyCurrArray_[TOTAL_KEYS];
	    bool keyPrevArray_[TOTAL_KEYS];
		POINT mouseScreenPositionCurr_;
	    POINT mouseScreenPositionPrev_;
	    int mouseWheelCurr_;
    public:
		// this resolves to specific mouse then calls its Triggered (return triggered result)
        static bool IsTriggered(const u32 key, const u32 index = 0);
        static bool IsReleased(const u32 key, const u32 index = 0);
        static bool IsPressed(const u32 key, const u32 index = 0);
        static bool IsDepressed(const u32 key, const u32 index = 0);
		static int  OnMouseWheel(const u32 index = 0);
	    static bool OnMouseMove(const u32 index = 0);
		static Vector2 GetMouseScreenPosition(const u32 index = 0);
		static Vector2 GetMouseClientPosition(const u32 index = 0);
        static Vector2 GetMouseNormalizedPosition(const u32 index = 0);
    public:
        MouseBase();

        S_NIMPL virtual bool Triggered(const u32) = 0; // actually does trigger
        S_NIMPL virtual bool Released(const u32) = 0;
        S_NIMPL virtual bool Pressed(const u32) = 0;
        S_NIMPL virtual bool Depressed(const u32) = 0;
		S_NIMPL virtual int  MouseWheel() = 0;
	    S_NIMPL virtual bool MouseMove() = 0;
		S_NIMPL virtual Vector2 MouseScreenPosition() = 0;
		S_NIMPL virtual Vector2 MouseClientPosition() = 0;
        S_NIMPL virtual Vector2 MouseNormalizedPosition() = 0;
        void Update(f32 dt);
    };
}

#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32
#include "WMouse.h"
#elif SIMIAN_PLATFORM == SIMIAN_PLATFORM_OSX
#elif SIMIAN_PLATFORM == SIMIAN_PLATFORM_LINUX
#endif

#endif
