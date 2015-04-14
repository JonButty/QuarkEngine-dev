#ifndef BOUNDINGVOLUMEFACTORY_H
#define BOUNDINGVOLUMEFACTORY_H

#include "singleton.h"
#include "factory.h"
#include "boundingVolumeTypes.h"
#include "configPhysicsDLL.h"
#include "boundingVolume.h"

namespace Phy
{
    class ENGINE_PHYSICS_EXPORT BoundingVolumeFactory
        :   public Util::Singleton<BoundingVolumeFactory>,
            public Util::Factory<BoundingVolume,BV_TOTAL>
    {};
}

#endif