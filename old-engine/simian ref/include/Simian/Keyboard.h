/************************************************************************/
/*!
\file		Keyboard.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_KEYBOARD_H_
#define SIMIAN_KEYBOARD_H_

#include <windows.h>
#include "Simian/InputDevice.h"
#include "Simian/TemplateFactory.h"

namespace Simian
{
    class Keyboard;

    class SIMIAN_EXPORT KeyboardBase: public InputDevice
    {
    public:
        enum Key
        {
            KEY_LBUTTON,
            KEY_RBUTTON,
            KEY_CANCEL,
            KEY_MBUTTON,
            KEY_XBUTTON1,
            KEY_XBUTTON2,
            KEY_BACKSPACE,
            KEY_TAB,
            KEY_CLEAR,
            KEY_RETURN,
            KEY_SHIFT,
            KEY_CONTROL,
            KEY_ALT,
            KEY_PAUSE,
            KEY_CAPS,
            KEY_KANA,
            KEY_HANGUEL,
            KEY_HANGUL,
            KEY_JUNJA,
            KEY_FINAL,
            KEY_HANJA,
            KEY_KANJI,
            KEY_ESCAPE,
            KEY_CONVERT,
            KEY_NONCONVERT,
            KEY_ACCEPT,
            KEY_MODECHANGE,
            KEY_SPACE,
            KEY_PAGEUP,
            KEY_NEXT,
            KEY_END,
            KEY_HOME,
            KEY_LEFT,
            KEY_UP,
            KEY_RIGHT,
            KEY_DOWN,
            KEY_SELECT,
            KEY_PRINT,
            KEY_EXECUTE1,
            KEY_PRINTSCREEN,
            KEY_INSERT,
            KEY_DELETE,
            KEY_HELP,
            KEY_0,
            KEY_1,
            KEY_2,
            KEY_3,
            KEY_4,
            KEY_5,
            KEY_6,
            KEY_7,
            KEY_8,
            KEY_9,
            KEY_A,
            KEY_B,
            KEY_C,
            KEY_D,
            KEY_E,
            KEY_F,
            KEY_G,
            KEY_H,
            KEY_I,
            KEY_J,
            KEY_K,
            KEY_L,
            KEY_M,
            KEY_N,
            KEY_O,
            KEY_P,
            KEY_Q,
            KEY_R,
            KEY_S,
            KEY_T,
            KEY_U,
            KEY_V,
            KEY_W,
            KEY_X,
            KEY_Y,
            KEY_Z,
            KEY_LWIN,
            KEY_RWIN,
            KEY_APPS,
            KEY_SLEEP,
            KEY_NUMPAD0,
            KEY_NUMPAD1,
            KEY_NUMPAD2,
            KEY_NUMPAD3,
            KEY_NUMPAD4,
            KEY_NUMPAD5,
            KEY_NUMPAD6,
            KEY_NUMPAD7,
            KEY_NUMPAD8,
            KEY_NUMPAD9,
            KEY_MULTIPLY,
            KEY_ADD,
            KEY_SEPARATOR,
            KEY_SUBTRACT,
            KEY_DECIMAL,
            KEY_DIVIDE,
            KEY_F1,
            KEY_F2,
            KEY_F3,
            KEY_F4,
            KEY_F5,
            KEY_F6,
            KEY_F7,
            KEY_F8,
            KEY_F9,
            KEY_F10,
            KEY_F11,
            KEY_F12,
            KEY_F13,
            KEY_F14,
            KEY_F15,
            KEY_F16,
            KEY_F17,
            KEY_F18,
            KEY_F19,
            KEY_F20,
            KEY_F21,
            KEY_F22,
            KEY_F23,
            KEY_F24,
            KEY_NUMLOCK,
            KEY_SCROLL,
            KEY_LSHIFT,
            KEY_RSHIFT,
            KEY_LCONTROL,
            KEY_RCONTROL,
            KEY_LMENU,
            KEY_RMENU,
            KEY_BROWSER_BACK,
            KEY_BROWSER_FORWARD,
            KEY_BROWSER_REFRESH,
            KEY_BROWSER_STOP,
            KEY_BROWSER_SEARCH,
            KEY_BROWSER_FAVORITES,
            KEY_BROWSER_HOME,
            KEY_VOLUME_MUTE,
            KEY_VOLUME_DOWN,
            KEY_VOLUME_UP,
            KEY_MEDIA_NEXT_TRACK,
            KEY_MEDIA_PREV_TRACK,
            KEY_MEDIA_STOP,
            KEY_MEDIA_PLAY_PAUSE,
            KEY_LAUNCH_MAIL,
            KEY_LAUNCH_MEDIA_SELECT,
            KEY_LAUNCH_APP1,
            KEY_LAUNCH_APP2,
            KEY_OEM_1 = VK_OEM_1,
            KEY_OEM_PLUS,
            KEY_OEM_COMMA,
            KEY_OEM_MINUS,
            KEY_OEM_PERIOD,
            KEY_OEM_2,
            KEY_OEM_3,
            KEY_OEM_4,
            KEY_OEM_5,
            KEY_OEM_6,
            KEY_OEM_7,
            KEY_OEM_8,
            KEY_OEM_102,
            KEY_PROCESSKEY,
            KEY_PACKET,
            KEY_ATTN,
            KEY_CRSEL,
            KEY_EXSEL,
            KEY_EREOF,
            KEY_PLAY,
            KEY_ZOOM,
            KEY_PA1,
            KEY_OEM_CLEAR,
            TOTAL_KEYS = 255
        };
    private:
        static FactoryPlant<InputDevice, Keyboard> factoryPlant_;
        typedef bool KeyMap[TOTAL_KEYS];
    protected:
        KeyMap keyCurrArray_;
	    KeyMap keyPrevArray_;
    public:
		// this resolves to specific keyboard then calls its Triggered (return triggered result)
        static bool IsTriggered(const u32 key, const u32 index = 0);
        static bool IsReleased(const u32 key, const u32 index = 0);
        static bool IsPressed(const u32 key, const u32 index = 0);
    public:
        KeyboardBase();

        S_NIMPL virtual bool Triggered(const u32) = 0; // actually does trigger
        S_NIMPL virtual bool Released(const u32) = 0;
        S_NIMPL virtual bool Pressed(const u32) = 0;
        void Update(f32 dt);
    };
}

#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32
#include "WKeyboard.h"
#elif SIMIAN_PLATFORM == SIMIAN_PLATFORM_OSX
#elif SIMIAN_PLATFORM == SIMIAN_PLATFORM_LINUX
#endif

#endif
