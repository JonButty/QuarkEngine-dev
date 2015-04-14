/************************************************************************/
/*!
\file		InputManager.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/InputManager.h"
#include "Simian/InputDevice.h"
#include <algorithm>
#include <iostream>
namespace Simian
{
    InputManager::InputManager()
        : Factory(DID_TOTAL),
		  type_(DID_MOUSE),
		  index_(0)
	{
        devices_.resize(DID_TOTAL);
	}

    //--------------------------------------------------------------------------

	InputManager::InputDeviceID InputManager::Type() const
	{
		return type_;
	}

	void InputManager::Type(InputManager::InputDeviceID id)
	{
		type_ = id;
	}

	u32 InputManager::Index() const
	{
		return index_;
	}

	void InputManager::Index(u32 index)
	{
		index_ = index;
	}

    //--------------------------------------------------------------------------

    void InputManager::OnCreateInstance(InputDevice* ip)
    {
        ip->Index(devices_[ip->Type()].size());
        devices_[ip->Type()].push_back(ip);
    }

    void InputManager::OnDestroyInstance(InputDevice* ip)
    {
        std::remove(devices_[ip->Type()].begin(), devices_[ip->Type()].end(), ip);
    }

    void InputManager::Update( f32 dt )
    {
        for (u32 i = 0; i < devices_.size(); ++i)
            for (u32 j = 0; j < devices_[i].size(); ++j)
                devices_[i][j]->Update(dt);
    }
}
