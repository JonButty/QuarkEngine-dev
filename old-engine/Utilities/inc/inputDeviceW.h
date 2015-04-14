#ifndef INPUTDEVICEW_H
#define INPUTDEVICEW_H

#include "configUtilitiesDLL.h"

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Util
{
    class ENGINE_UTILITIES_EXPORT InputDevice
    {
	public:
		struct UpdateInfo
		{
			HWND hwnd_;
			UINT msg_;
            WPARAM wParam_;
            LPARAM lParam_;
		};
    public:
        InputDevice(unsigned int type,
                    unsigned int maxKeys);
        virtual ~InputDevice();
        bool Pressed(unsigned int key) const;
        bool Depressed(unsigned int key) const;
        bool Released(unsigned int key) const;
        bool Triggered(unsigned int key) const;
        void Update(const UpdateInfo& info);
    public:
        bool Enable() const;
        void Enable(bool val);
        unsigned int Id() const;
        void Id(unsigned int val);
        unsigned int Type() const;
        std::string Name() const;
        void Name(std::string val);
    protected:
        virtual void update_(const UpdateInfo& info) = 0;
        virtual unsigned int getMappedKeyValue_(unsigned int val) const = 0;
    private:
        InputDevice(const InputDevice&);
    protected:
        typedef bool* KeyList;
    protected:
		KeyList curr_;
        KeyList prev_;
        bool enable_;
        unsigned int id_;
        unsigned int type_;
        unsigned int maxKeys_;
        std::string name_;
    };
}

#endif