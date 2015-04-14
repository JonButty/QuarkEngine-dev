#ifndef DEBUGOBBDX10_H
#define DEBUGOBBDX10_H

#include "configGraphicsDLL.h"
#include "debugShape.h"
#include "debugShapeFactory.h"
#include "vector3.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT DebugOBB
        :   public DebugShape
    {
    public:
        DebugOBB();
        DebugOBB(const DebugOBB& val);
        ~DebugOBB();
    private:
        void load();
        void update();
        void unload();
        void updateInfo(Vertex*& list);
    private:
        static Util::FactoryPlant<DebugShape,DebugOBB>factoryPlant_;
    };
}

#endif