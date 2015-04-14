#include "inputDevice.h"

namespace Util
{
    InputDevice::InputDevice( unsigned int type,
                              unsigned int maxKeys) 
                              :	curr_(0),
								prev_(0),
								enable_(true),
                                id_(0),
								type_(type),
                                maxKeys_(maxKeys)
    {
        curr_ = new bool[maxKeys];
        prev_ = new bool[maxKeys];

        for(unsigned int i = 0; i < maxKeys_; ++i)
        {
            curr_[i] = false;
            prev_[i] = false;
        }
    }

    InputDevice::~InputDevice()
    {
        if(curr_)
        {
			delete [] curr_;
			curr_ = 0;
		}
        if(prev_)
        {
			delete [] prev_;
			prev_ = 0;
		}
    }

    bool InputDevice::Pressed( unsigned int key ) const
    {
        return curr_[key];
    }

    bool InputDevice::Depressed( unsigned int key ) const
    {
        return !prev_[key] && !curr_[key];
    }

    bool InputDevice::Released( unsigned int key ) const
    {
        return prev_[key] && !curr_[key];
    }

    bool InputDevice::Triggered( unsigned int key ) const
    {
        return !prev_[key] && curr_[key];
    }

    void InputDevice::Update(const UpdateInfo& info)
    {
        if(!enable_)
            return;

        for (unsigned int i = 0; i < maxKeys_; ++i)
        {
            prev_[i] = curr_[i];
            short val = GetAsyncKeyState(getMappedKeyValue_(i));
            if (val)
                curr_[i] = true;
            else
                curr_[i] = false;
        }

        update_(info);
    }

    //--------------------------------------------------------------------------

    void InputDevice::Enable( bool val )
    {
        enable_ = val;
    }

    bool InputDevice::Enable() const
    {
        return enable_;
    }

    unsigned int InputDevice::Id() const
    {
        return id_;
    }

    void InputDevice::Id(unsigned int val)
    {
        id_ = val;
    }

    unsigned int InputDevice::Type() const
    {
        return type_;
    }

    void InputDevice::Name(std::string val)
    {
        name_ = val;
    }

    std::string InputDevice::Name() const
    {
        return name_;
    }

	//--------------------------------------------------------------------------
	
	InputDevice::InputDevice(const InputDevice&)
	{
	}
}