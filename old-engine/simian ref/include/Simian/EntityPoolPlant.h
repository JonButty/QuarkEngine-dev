/************************************************************************/
/*!
\file		EntityPoolPlant.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_ENTITY_POOL_H_
#define SIMIAN_ENTITY_POOL_H_

#include "SimianPlatform.h"
#include "MemoryPool.h"

namespace Simian
{
    class EntityPool
    {
    private:
        MemoryPool<Entity> pool_;

    public:
        virtual Entity* Create
    };
}

#endif
