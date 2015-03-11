#ifndef MULTIINPUTDEVICEW_H
#define	MULTIINPUTDEVICEW_H

#include "inputDevice.h"
#include "configUtilitiesDLL.h"

namespace Util
{
	class ENGINE_UTILITIES_EXPORT MultiInputDevice
		:	public InputDevice
	{
	public:
		MultiInputDevice(unsigned int type,
						 unsigned int maxKeys);
        virtual ~MultiInputDevice();
	public:
		unsigned int Type() const;
	protected:
		virtual void update_(const UpdateInfo& info) = 0;
        virtual unsigned int getMappedKeyValue_(unsigned int val) const = 0;
	private:
		MultiInputDevice(const MultiInputDevice& val);
	};
}

#endif