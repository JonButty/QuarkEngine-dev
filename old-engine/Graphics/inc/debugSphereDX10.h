#ifndef DEBUGSPHEREDX10_H
#define DEBUGSPHEREDX10_H

#include "meshDX10.h"
#include "configGraphicsDLL.h"
#include "debugShape.h"
#include "debugShapeFactory.h"
#include "vector3.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT DebugSphere
        :   public DebugShape
    {
    public:
        DebugSphere();
        DebugSphere(const DebugSphere& val);
        ~DebugSphere();
    private:
        void load();
        void update();
        void unload();
        void updateInfo(Vertex*& list);
    private:
        static Util::FactoryPlant<DebugShape,DebugSphere>factoryPlant_;
    };
}

#endif