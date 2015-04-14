/*************************************************************************/
/*!
\file		EntityCollision.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_ENTITYCOLLISION_H_
#define DESCENSION_ENTITYCOLLISION_H_

#include "Collision.h"

namespace Descension
{
    class EntityCollision
    {
    public:
        //Checks for collision between entities
        static bool EntityCollision::CheckEntityCollision(Simian::Vector3 &currPosition,
            Simian::Vector3 &currVelocity,
            const Simian::Vector3 &targetPosition,
            const Simian::Vector3 &targetVelocity,
            const Simian::f32 &currRadius,
            const Simian::f32 &targetRadius,
            const Simian::f32 &dt,
            const bool &response = true); //default = no response
    };
}

#endif
