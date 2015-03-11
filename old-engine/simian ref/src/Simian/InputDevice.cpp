/************************************************************************/
/*!
\file		InputDevice.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/InputDevice.h"

namespace Simian
{
    InputDevice::InputDevice(u32 type)
		: type_(type)
	{
	}

	InputDevice::~InputDevice()
    {
    }

    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------

    u32 InputDevice::Type() const
    {
        return type_;
    }

	void InputDevice::Type(const u32 type)
    {
        type_ = type;
    }

    u32 InputDevice::Index() const
    {
        return index_;
    }

    void InputDevice::Index(const u32 index)
    {
        index_ = index;
    }

    //--------------------------------------------------------------------------
    
    void InputDevice::Update(f32)
    {
    }
}
