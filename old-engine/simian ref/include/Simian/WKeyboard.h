/************************************************************************/
/*!
\file		WKeyboard.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_WKEYBOARD_H_
#define SIMIAN_WKEYBOARD_H_

#include "Simian/SimianPlatform.h"
#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32

#include <windows.h>
#include "Simian/Keyboard.h"

namespace Simian
{
    class SIMIAN_EXPORT Keyboard: public KeyboardBase
    {
    public:
        enum Key
        {
            KEY_LBUTTON = VK_LBUTTON,
            KEY_RBUTTON = VK_RBUTTON,
            KEY_CANCEL = VK_CANCEL,
            KEY_MBUTTON = VK_MBUTTON,
            KEY_XBUTTON1 = VK_XBUTTON1,
            KEY_XBUTTON2 = VK_XBUTTON2,
            KEY_BACKSPACE = VK_BACK,
            KEY_TAB = VK_TAB,
            KEY_CLEAR = VK_CLEAR,
            KEY_RETURN = VK_RETURN,
            KEY_SHIFT = VK_SHIFT,
            KEY_CONTROL = VK_CONTROL,
            KEY_ALT = VK_MENU,
            KEY_PAUSE = VK_PAUSE,
            KEY_CAPS = VK_CAPITAL,
            KEY_KANA = VK_KANA,
            KEY_HANGUEL = VK_HANGEUL,
            KEY_HANGUL = VK_HANGUL,
            KEY_JUNJA = VK_JUNJA,
            KEY_FINAL = VK_FINAL,
            KEY_HANJA = VK_HANJA,
            KEY_KANJI = VK_KANJI,
            KEY_ESCAPE = VK_ESCAPE,
            KEY_CONVERT = VK_CONVERT,
            KEY_NONCONVERT = VK_NONCONVERT,
            KEY_ACCEPT = VK_ACCEPT,
            KEY_MODECHANGE = VK_MODECHANGE,
            KEY_SPACE = VK_SPACE,
            KEY_PAGEUP = VK_PRIOR,
            KEY_NEXT = VK_NEXT,
            KEY_END = VK_END,
            KEY_HOME = VK_HOME,
            KEY_LEFT = VK_LEFT,
            KEY_UP = VK_UP,
            KEY_RIGHT = VK_RIGHT,            
            KEY_DOWN = VK_DOWN,
            KEY_SELECT = VK_SELECT,
            KEY_PRINT = VK_PRINT,
            KEY_EXECUTE1 = VK_EXECUTE,
            KEY_PRINTSCREEN = VK_SNAPSHOT,
            KEY_INSERT = VK_INSERT,
            KEY_DELETE = VK_DELETE,
            KEY_HELP = VK_HELP,
            KEY_0 = 0x30,
            KEY_1 = 0x31,
            KEY_2 = 0x32,
            KEY_3 = 0x33,
            KEY_4 = 0x34,
            KEY_5 = 0x35,
            KEY_6 = 0x36,
            KEY_7 = 0x37,
            KEY_8 = 0x38,
            KEY_9 = 0x39,
            KEY_A = 0x41,
            KEY_B = 0x42,
            KEY_C = 0x43,
            KEY_D = 0x44,
            KEY_E = 0x45,
            KEY_F = 0x46,
            KEY_G = 0x47,
            KEY_H = 0x48,
            KEY_I = 0x49,
            KEY_J = 0x4A,
            KEY_K = 0x4B,
            KEY_L = 0x4C,
            KEY_M = 0x4D,
            KEY_N = 0x4E,
            KEY_O = 0x4F,
            KEY_P = 0x50,
            KEY_Q = 0x51,
            KEY_R = 0x52,
            KEY_S = 0x53,
            KEY_T = 0x54,
            KEY_U = 0x55,
            KEY_V = 0x56,
            KEY_W = 0x57,
            KEY_X = 0x58,
            KEY_Y = 0x59,
            KEY_Z = 0x5A,
            KEY_LWIN = VK_LWIN,
            KEY_RWIN = VK_RWIN,
            KEY_APPS = VK_APPS,
            KEY_SLEEP = VK_SLEEP,
            KEY_NUMPAD0 = VK_NUMPAD0,
            KEY_NUMPAD1 = VK_NUMPAD1,
            KEY_NUMPAD2 = VK_NUMPAD2,
            KEY_NUMPAD3 = VK_NUMPAD3,
            KEY_NUMPAD4 = VK_NUMPAD4,
            KEY_NUMPAD5 = VK_NUMPAD5,
            KEY_NUMPAD6 = VK_NUMPAD6,
            KEY_NUMPAD7 = VK_NUMPAD7,
            KEY_NUMPAD8 = VK_NUMPAD8,
            KEY_NUMPAD9 = VK_NUMPAD9,
            KEY_MULTIPLY = VK_MULTIPLY,
            KEY_ADD = VK_ADD,
            KEY_SEPARATOR = VK_SEPARATOR,
            KEY_SUBTRACT = VK_SUBTRACT,
            KEY_DECIMAL = VK_DECIMAL,
            KEY_DIVIDE = VK_DIVIDE,
            KEY_F1 = VK_F1,
            KEY_F2 = VK_F2,
            KEY_F3 = VK_F3,
            KEY_F4 = VK_F4,
            KEY_F5 = VK_F5,
            KEY_F6 = VK_F6,
            KEY_F7 = VK_F7,
            KEY_F8 = VK_F8,
            KEY_F9 = VK_F9,
            KEY_F10 = VK_F10,
            KEY_F11 = VK_F11,
            KEY_F12 = VK_F12,
            KEY_F13 = VK_F13,
            KEY_F14 = VK_F14,
            KEY_F15 = VK_F15,
            KEY_F16 = VK_F16,
            KEY_F17 = VK_F17,
            KEY_F18 = VK_F18,
            KEY_F19 = VK_F19,
            KEY_F20 = VK_F20,
            KEY_F21 = VK_F21,
            KEY_F22 = VK_F22,
            KEY_F23 = VK_F23,
            KEY_F24 = VK_F24,
            KEY_NUMLOCK = VK_NUMLOCK,
            KEY_SCROLL = VK_SCROLL,
            KEY_LSHIFT = VK_LSHIFT,
            KEY_RSHIFT = VK_RSHIFT,
            KEY_LCONTROL = VK_LCONTROL,
            KEY_RCONTROL = VK_RCONTROL,
            KEY_LMENU = VK_LMENU,
            KEY_RMENU = VK_RMENU,
            KEY_BROWSER_BACK = VK_BROWSER_BACK,
            KEY_BROWSER_FORWARD = VK_BROWSER_FORWARD,
            KEY_BROWSER_REFRESH = VK_BROWSER_REFRESH,
            KEY_BROWSER_STOP = VK_BROWSER_STOP,
            KEY_BROWSER_SEARCH = VK_BROWSER_SEARCH,
            KEY_BROWSER_FAVORITES = VK_BROWSER_FAVORITES,
            KEY_BROWSER_HOME = VK_BROWSER_HOME,
            KEY_VOLUME_MUTE = VK_VOLUME_MUTE,
            KEY_VOLUME_DOWN = VK_VOLUME_DOWN,
            KEY_VOLUME_UP = VK_VOLUME_UP,
            KEY_MEDIA_NEXT_TRACK = VK_MEDIA_NEXT_TRACK,
            KEY_MEDIA_PREV_TRACK = VK_MEDIA_PREV_TRACK,
            KEY_MEDIA_STOP = VK_MEDIA_STOP,
            KEY_MEDIA_PLAY_PAUSE = VK_MEDIA_PLAY_PAUSE,
            KEY_LAUNCH_MAIL = VK_LAUNCH_MAIL,
            KEY_LAUNCH_MEDIA_SELECT = VK_LAUNCH_MEDIA_SELECT,
            KEY_LAUNCH_APP1 = VK_LAUNCH_APP1,
            KEY_LAUNCH_APP2 = VK_LAUNCH_APP2,
            KEY_OEM_1 = VK_OEM_1,
            KEY_OEM_PLUS = VK_OEM_PLUS,
            KEY_OEM_COMMA = VK_OEM_COMMA,
            KEY_OEM_MINUS = VK_OEM_MINUS,
            KEY_OEM_PERIOD = VK_OEM_PERIOD,
            KEY_OEM_2 = VK_OEM_2,
            KEY_OEM_3 = VK_OEM_3,
            KEY_OEM_4 = VK_OEM_4,
            KEY_OEM_5 = VK_OEM_5,
            KEY_OEM_6 = VK_OEM_6,
            KEY_OEM_7 = VK_OEM_7,
            KEY_OEM_8 = VK_OEM_8,
            KEY_OEM_102 = VK_OEM_102,
            KEY_PROCESSKEY = VK_PROCESSKEY,
            KEY_PACKET = VK_PACKET,
            KEY_ATTN = VK_ATTN,
            KEY_CRSEL = VK_CRSEL,
            KEY_EXSEL = VK_EXSEL,
            KEY_EREOF = VK_EREOF,
            KEY_PLAY = VK_PLAY,
            KEY_ZOOM = VK_ZOOM,
            KEY_PA1 = VK_PA1,
            KEY_OEM_CLEAR = VK_OEM_CLEAR,
            TOTAL_KEYS
        };
    private:
        
    public:
        bool Pressed(const u32 key);
        bool Triggered(const u32 key);
        bool Released(const u32 key);
    public:
        
        void Update(f32 dt);
    };
}

#endif

#endif
