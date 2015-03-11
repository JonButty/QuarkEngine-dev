/************************************************************************/
/*!
\file		WMouse.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/SimianPlatform.h"
#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32
#include "Simian/Mouse.h"
#include "Simian/LogManager.h"
#include <iostream>

namespace Simian
{
    // pimpl for os hooks
    struct OSHookParameter: public DelegateParameter
    {
        UINT Msg;
        LPARAM LParam;
        WPARAM WParam;
        bool Handled;
    };
    void registerOSHook_(const Delegate& delegate);

    Mouse::Mouse()
		: numEvents_(0),
		  numEventsRead_(0),
		  winHand_(0)
	{
        registerOSHook_(Simian::Delegate::CreateDelegate<Mouse, &Mouse::mouseWheelUpdate_>(this));
	}

    //--------------------------------------------------------------------------

    void Mouse::mouseWheelUpdate_( DelegateParameter& param )
    {
        OSHookParameter& hookParam = param.As<OSHookParameter>();
        if (hookParam.Msg == WM_MOUSEWHEEL)
        {
            short delta = GET_WHEEL_DELTA_WPARAM(hookParam.WParam);
            mouseWheelCurr_ = delta;
            hookParam.Handled = true;
        }
    }

    //--------------------------------------------------------------------------

    bool Mouse::Pressed(const u32 key)
	{
		return (keyPrevArray_[key] && keyCurrArray_[key]);
	}

    bool Mouse::Depressed( const u32 key )
    {
        return (!keyPrevArray_[key] && !keyCurrArray_[key]);
    }

	bool Mouse::Triggered(const u32 key)
	{
		return (!keyPrevArray_[key] && keyCurrArray_[key]);
	}

	bool Mouse::Released(const u32 key)
	{
		return (keyPrevArray_[key] && !keyCurrArray_[key]);
	}

	int Mouse::MouseWheel()
	{
		return mouseWheelCurr_;
	}

	bool Mouse::MouseMove()
	{
		if (mouseScreenPositionCurr_.x != mouseScreenPositionPrev_.x
			|| mouseScreenPositionCurr_.y != mouseScreenPositionPrev_.y)
			return true;
		return false;
	}

	Vector2 Mouse::MouseScreenPosition()
	{
		return Vector2(
            static_cast<f32>(mouseScreenPositionCurr_.x), 
            static_cast<f32>(mouseScreenPositionCurr_.y));
	}

	Vector2 Mouse::MouseClientPosition()
	{
		POINT tempPoint(mouseScreenPositionCurr_);
		ScreenToClient(winHand_, &tempPoint);
        return Vector2(
            static_cast<f32>(tempPoint.x), 
            static_cast<f32>(tempPoint.y));
    }

    Simian::Vector2 Mouse::MouseNormalizedPosition()
    {
        RECT rect;
        GetClientRect(winHand_, &rect);
        return MouseClientPosition()/Vector2(static_cast<f32>(rect.right - rect.left), 
                                             static_cast<f32>(rect.top - rect.bottom))
                                             * 2.0f - Vector2(1.0f, -1.0f);
    }

	void Mouse::WinHand(WindowHandle wh)
	{
		winHand_ = *reinterpret_cast<HWND*>(wh);
	}

    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------

    void Mouse::Update(f32)
    {
        for (int i=1; i<5; ++i)
		{
			keyPrevArray_[i] = keyCurrArray_[i];
			if (GetAsyncKeyState(i) & 0x8000)
				keyCurrArray_[i] = true;
			else
				keyCurrArray_[i] = false;
		}
		
		mouseWheelCurr_ = 0;
		if (winHand_)
		{
			mouseScreenPositionPrev_ = mouseScreenPositionCurr_;
			GetCursorPos(&mouseScreenPositionCurr_);
		}
    }
}

#endif
