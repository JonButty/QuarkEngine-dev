#include "singleInputDevice.h"
#include "configOS.h"

#if ENGINE_OS == ENGINE_OS_WINDOWS

namespace Util
{
	SingleInputDevice::SingleInputDevice(unsigned int type,
										 unsigned int maxKeys)
										 :	InputDevice(type,
														maxKeys)
	{
	}

	SingleInputDevice::~SingleInputDevice()
	{
	}
	
	//--------------------------------------------------------------

	SingleInputDevice::SingleInputDevice(const SingleInputDevice& val)
	{
	}
}

#endif