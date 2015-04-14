/************************************************************************/
/*!
\file		InputDevice.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_INPUTDEVICE_H
#define SIMIAN_INPUTDEVICE_H

#include "SimianPlatform.h"
#include "InputManager.h"

namespace Simian
{
    class SIMIAN_EXPORT InputDevice
    {
    private:
        u32 type_, index_;
    public:
        u32 Type() const;
		void Type(const u32 index);
        u32 Index() const;
        void Index(const u32 index);
    public:
        InputDevice(u32 type);
		virtual ~InputDevice();
        virtual void Update(f32 dt);
    };
}

#endif

