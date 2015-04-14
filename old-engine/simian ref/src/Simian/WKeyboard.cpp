/************************************************************************/
/*!
\file		WKeyboard.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Keyboard.h"

#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32

namespace Simian
{
    //--------------------------------------------------------------------------

    bool Keyboard::Pressed(const u32 key)
	{
		return (keyPrevArray_[key] && keyCurrArray_[key]);
	}

	// Triggered function
	bool Keyboard::Triggered(const u32 key)
	{
		return (!keyPrevArray_[key] && keyCurrArray_[key]);
	}

	// Released function
	bool Keyboard::Released(const u32 key)
	{
		return (keyPrevArray_[key] && !keyCurrArray_[key]);
	}

    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------

    void Keyboard::Update(f32)
    {
        for (u32 i=0; i<TOTAL_KEYS; ++i)
		{
			keyPrevArray_[i] = keyCurrArray_[i];
			if (GetAsyncKeyState(i) & 0x8000)
				keyCurrArray_[i] = true;
			else
				keyCurrArray_[i] = false;
		}
    }
}

#endif
