#ifndef DEBUGSHAPEFACTORY_H
#define DEBUGSHAPEFACTORY_H

#include "configGraphicsDLL.h"
#include "singleton.h"
#include "factory.h"
#include "debugShape.h"
#include "debugShapeTypes.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT DebugShapeFactory
        :   public Util::Factory<DebugShape,DS_TOTAL>,
            public Util::Singleton<DebugShapeFactory>
    {
	private:
		void onCreateInstance(DebugShape* val);
		void onDestroyInstance(DebugShape* val);
	};
}

#endif