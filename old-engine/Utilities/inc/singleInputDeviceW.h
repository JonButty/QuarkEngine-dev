#ifndef SINGLEINPUTDEVICEW_H
#define	SINGLEINPUTDEVICEW_H

#include "inputDevice.h"
#include "configUtilitiesDLL.h"

namespace Util
{
	class ENGINE_UTILITIES_EXPORT SingleInputDevice
		:	public InputDevice
	{
	public:
		SingleInputDevice(unsigned int type,
						  unsigned int maxKeys);
        virtual ~SingleInputDevice();
	protected:
		virtual void update_(const UpdateInfo& info) = 0;
        virtual unsigned int getMappedKeyValue_(unsigned int val) const = 0;
	private:
		SingleInputDevice(const SingleInputDevice& val);
	};
}

#endif