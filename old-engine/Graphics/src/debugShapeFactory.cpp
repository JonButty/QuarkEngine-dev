#include "debugShapeFactory.h"

namespace GFX
{
	void DebugShapeFactory::onCreateInstance(DebugShape* val)
	{
		val->Load();
	}

	void DebugShapeFactory::onDestroyInstance(DebugShape* val)
	{
		val->Unload();
	}
}