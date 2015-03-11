/************************************************************************/
/*!
\file		SMDefaultStrategy.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/SMDefaultStrategy.h"
#include "Simian/CCameraBase.h"
#include "Simian/CCamera.h"
#include "Simian/Camera.h"
#include "Simian/AABB.h"
#include "Simian/Scene.h"

namespace Simian
{
    void SMDefaultStrategy::cullEntity_( Camera* cam, Entity* entity )
    {
        // if i can't see the AABB then we just stop (don't cull volumeless objects)
        if (entity->AABB().Exists() && !cam->TestAABB(entity->AABB()))
            return;

        // if not we say the entity is visible if it is indeed cullable...
        if (entity->Cullable()) 
            Scene()->AddVisibleEntity(entity);

        // then recurse children!
        for (u32 i = 0; i < entity->Children().size(); ++i)
            cullEntity_(cam, entity->Children()[i]);
    }

    //--------------------------------------------------------------------------

    void SMDefaultStrategy::Cull()
    {
        CCamera* cam;
        CCameraBase::SceneCamera(cam);

        // cannot cull if there are no cameras.
        if (!cam) return;

        // go through root entities and find if the AABB intersects with the camera
        for (u32 i = 0; i < RootEntities().size(); ++i)
            cullEntity_(&cam->Camera(), RootEntities()[i]);
    }
}
