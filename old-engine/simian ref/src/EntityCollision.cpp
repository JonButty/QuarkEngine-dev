/*************************************************************************/
/*!
\file		EntityCollision.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "EntityCollision.h"

namespace Descension
{
    bool EntityCollision::CheckEntityCollision(Simian::Vector3 &currPosition,
        Simian::Vector3 &currVelocity,
        const Simian::Vector3 &targetPosition,
        const Simian::Vector3 &targetVelocity,
        const Simian::f32 &currRadius,
        const Simian::f32 &targetRadius,
        const Simian::f32 &dt,
        const bool &response)
    {
        //referenced variables
        Simian::f32 getInterTime = dt;
        Simian::Vector3 velAB = currVelocity - targetVelocity;

        //check circle overlap with time
        if ( Collision::CircleCollision( currPosition,
            velAB,
            currRadius + targetRadius,
            targetPosition,
            getInterTime,
            response) )
        {
                //OVERLOADED RESPONSE: this has radius transfer
                //Collision::CircleCollisionResponse(currPosition,
                //    currVelocity,
                //    currRadius,
                //    targetNewPos,
                //    targetVelocity,
                //    targetRadius,
                //    targetNewPos,
                //    getInterTime);
            currVelocity = velAB;

            return true;
        }
        return false;
    }
}
