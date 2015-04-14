/************************************************************************/
/*!
\file		Keyboard.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Keyboard.h"

namespace Simian
{
    FactoryPlant<InputDevice, Keyboard> Keyboard::factoryPlant_
        (InputManager::InstancePtr(), InputManager::DID_KEYBOARD);

    bool KeyboardBase::IsTriggered(const u32 key, const u32 index)
    {
        Keyboard* keyboard;
        Simian::InputManager::Instance().GetDevice(Simian::InputManager::DID_KEYBOARD, index, keyboard);
        return keyboard->Triggered(key);
    }

    bool KeyboardBase::IsReleased(const u32 key, const u32 index)
    {
        Keyboard* keyboard;
        Simian::InputManager::Instance().GetDevice(Simian::InputManager::DID_KEYBOARD, index, keyboard);
        return keyboard->Released(key);
    }

    bool KeyboardBase::IsPressed(const u32 key, const u32 index)
    {
        Keyboard* keyboard;
        Simian::InputManager::Instance().GetDevice(Simian::InputManager::DID_KEYBOARD, index, keyboard);
        return keyboard->Pressed(key);
    }

	//--------------------------------------------------------------------------

    KeyboardBase::KeyboardBase()
		: InputDevice(Simian::InputManager::DID_KEYBOARD)
	{
		for (u32 i=0; i < TOTAL_KEYS; ++i)
		{
			keyCurrArray_[i] = false;
			keyPrevArray_[i] = false;
		}
	}

    //--------------------------------------------------------------------------

    

    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------

    void KeyboardBase::Update(f32)
    {
    }
}
