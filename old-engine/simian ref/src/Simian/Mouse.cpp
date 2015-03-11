/************************************************************************/
/*!
\file		Mouse.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Mouse.h"

namespace Simian
{
    FactoryPlant<InputDevice, Mouse> Mouse::factoryPlant_
        (InputManager::InstancePtr(), InputManager::DID_MOUSE);

    bool MouseBase::IsTriggered(const u32 key, const u32 index)
    {
        Mouse* mouse;
        Simian::InputManager::Instance().GetDevice(Simian::InputManager::DID_MOUSE, index, mouse);
        return mouse->Triggered(key);
    }

    bool MouseBase::IsReleased(const u32 key, const u32 index)
    {
        Mouse* mouse;
        Simian::InputManager::Instance().GetDevice(Simian::InputManager::DID_MOUSE, index, mouse);
        return mouse->Released(key);
    }

    bool MouseBase::IsPressed(const u32 key, const u32 index)
    {
        Mouse* mouse;
        Simian::InputManager::Instance().GetDevice(Simian::InputManager::DID_MOUSE, index, mouse);
        return mouse->Pressed(key);
    }

    bool MouseBase::IsDepressed( const u32 key, const u32 index /*= 0*/ )
    {
        Mouse* mouse;
        Simian::InputManager::Instance().GetDevice(Simian::InputManager::DID_MOUSE, index, mouse);
        return mouse->Depressed(key);
    }

	int MouseBase::OnMouseWheel(const u32 index)
    {
        Mouse* mouse;
        Simian::InputManager::Instance().GetDevice(Simian::InputManager::DID_MOUSE, index, mouse);
        return mouse->MouseWheel();
    }

    bool MouseBase::OnMouseMove(const u32 index)
    {
        Mouse* mouse;
        Simian::InputManager::Instance().GetDevice(Simian::InputManager::DID_MOUSE, index, mouse);
        return mouse->MouseMove();
    }

	Vector2 MouseBase::GetMouseScreenPosition(const u32 index)
    {
        Mouse* mouse;
        Simian::InputManager::Instance().GetDevice(Simian::InputManager::DID_MOUSE, index, mouse);
        return mouse->MouseScreenPosition();
    }

	Vector2 MouseBase::GetMouseClientPosition(const u32 index)
    {
        Mouse* mouse;
        Simian::InputManager::Instance().GetDevice(Simian::InputManager::DID_MOUSE, index, mouse);
        return mouse->MouseClientPosition();
    }

    Vector2 MouseBase::GetMouseNormalizedPosition(const u32 index)
    {
        Mouse* mouse;
        Simian::InputManager::Instance().GetDevice(Simian::InputManager::DID_MOUSE, index, mouse);
        return mouse->MouseNormalizedPosition();
    }

	//--------------------------------------------------------------------------

    MouseBase::MouseBase()
		: InputDevice(Simian::InputManager::DID_MOUSE),
		  mouseWheelCurr_(0)
	{
		for (u32 i=0; i < TOTAL_KEYS; ++i)
		{
			keyCurrArray_[i] = false;
			keyPrevArray_[i] = false;
		}
		mouseScreenPositionCurr_.x = 0;
		mouseScreenPositionCurr_.y = 0;
		mouseScreenPositionPrev_.x = 1;
		mouseScreenPositionPrev_.y = 1;
	}
	
    //--------------------------------------------------------------------------

    

    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------

    void MouseBase::Update(f32)
    {
    }
}
