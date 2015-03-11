/************************************************************************/
/*!
\file		InputManager.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_INPUTMANAGER_H
#define SIMIAN_INPUTMANAGER_H

#include "Simian/SimianPlatform.h"
#include "Simian/TemplateFactory.h"
#include <vector>

namespace Simian
{
    class InputDevice;

    class SIMIAN_EXPORT InputManager
        : public Singleton<InputManager>,
          public Factory<InputDevice>
    {
    public:
        enum InputDeviceID
        {
			DID_MOUSE,
            DID_KEYBOARD,
            DID_TOTAL
        };
    private:
        std::vector< std::vector<InputDevice*> > devices_;
		InputDeviceID type_;
		u32 index_;

    public:
        template <class T>
		void GetDevice(const InputDeviceID id, const u32 index, T*& devicePointer)
		{
			devicePointer = reinterpret_cast<T*>(devices_[id][index]);
		}

	public:
		InputDeviceID Type() const;
		void Type(InputDeviceID id);
		u32 Index() const;
		void Index(u32 index);
    public:
        InputManager();
        
        void OnCreateInstance(InputDevice* ip);
        void OnDestroyInstance(InputDevice* ip);

        void Update(f32 dt);
    };

	SIMIAN_IMPORT template class SIMIAN_EXPORT Singleton<InputManager>;
}

#endif
