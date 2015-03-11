#ifndef DEBUGELLIPSEDX10_H
#define DEBUGELLIPSEDX10_H

#include "meshDX10.h"
#include "configGraphicsDLL.h"
#include "debugShape.h"
#include "debugShapeFactory.h"
#include "vector3.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT DebugEllipse
        :   public DebugShape
    {
    public:
        DebugEllipse();
        DebugEllipse(const DebugEllipse& val);
        ~DebugEllipse();
    private:
        void load();
        void update();
        void unload();
        void updateInfo(Vertex*& list);
    private:
        static Util::FactoryPlant<DebugShape,DebugEllipse>factoryPlant_;
    };
}

#endif