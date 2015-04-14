#ifndef DEBUGAABBDX10_H
#define DEBUGAABBDX10_H

#include "configGraphicsDLL.h"
#include "debugShape.h"
#include "debugShapeFactory.h"
#include "vector3.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT DebugAABB
        :   public DebugShape
    {
    public:
        DebugAABB();
        DebugAABB(const DebugAABB& val);
        ~DebugAABB();
    private:
        void load();
        void update();
        void unload();
        void updateInfo(Vertex*& list);
    private:
        static Util::FactoryPlant<DebugShape,DebugAABB>factoryPlant_;
    };
}

#endif