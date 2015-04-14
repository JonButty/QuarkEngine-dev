/*************************************************************************/
/*!
\file		Collision.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_COLLISION_H_
#define DESCENSION_COLLISION_H_

#include "Simian/Vector3.h"
#include "Simian/SimianTypes.h"
#include "Simian/Math.h"
#include "Simian/Vector2.h"
#include "Simian/Ray.h"

#include "TileWallCollision.h"

namespace Descension
{
    //Collision class for game play
    
    class Collision
    {
    public:
        enum COLLISION_ID
        {
            COLLISION_L0 = 0, // tiles & walls
            COLLISION_L1,
            COLLISION_L2,
            COLLISION_L3,
            COLLISION_L4,
            COLLISION_L5,
            COLLISION_L6,
            COLLISION_L7,

            COLLISION_TOTAL_LAYERS
        };

    public:
        //Closest an object can get to a surface
        static const Simian::f32 MIN_DIST, MAX_PENETRATION, PUSH_BACK,
                                 DT_MULTIPLIER;
        
    public:
        /* Checks a ray against a wall with its current velocity
        , returns true if collided with updated position */
        static bool LineCollision(Simian::Vector3 &currPosition,
                            Simian::Vector3 &currVelocity,
                            const Simian::f32 &radius,
                            const Simian::Vector3 &wallPt0,
                            const Simian::Vector3 &wallPt1,
                            const Simian::Vector3 &wallNormal,
                            const bool &response = true);

        /* Response to line contact */
        static void LineCollisionResponse(Simian::Vector3 &currPosition,
                            const Simian::Vector3 &interPt,
                            const Simian::Vector3 &currVelocity,
                            const Simian::Vector3 &wallNormal);

        /* Checks a ray against a circle of accumulated radius
        for collision, returns true if collided with updated position */
        static bool CircleCollision(Simian::Vector3 &currPosition,
                            Simian::Vector3 &currVelocity,
                            const Simian::f32 &radius,
                            const Simian::Vector3 &circleCenter,
                            Simian::f32 &getInterTime,
                            const bool &response = true);

        /* Response to circle contact */
        static void CircleCollisionResponse( Simian::Vector3 &currPosition,
                            Simian::Vector3 &currVelocity,
                            const Simian::f32 &interTime,
                            const Simian::Vector3 &circlePos,
                            const Simian::f32 &radius);

        /* Response to entity vs. entity contact */
        static void CircleCollisionResponse(
                            //your components
                            Simian::Vector3 &interPtA,
                            Simian::Vector3 &velA,
                            const Simian::f32 &radiusA,
                            //their components
                            const Simian::Vector3 &interPtB,
                            const Simian::Vector3 &velB,
                            const Simian::f32 &radiusB,
                            //what was the relative time of the intersection?
                            const Simian::Vector3 &circlePos,
                            const Simian::f32 &interTime);

        static bool PointAABB2D(const Simian::Vector2& p, 
                                const Simian::Vector2& min,
                                const Simian::Vector2& max);

        static bool LineAABB2D(const Simian::Vector2& p, 
                               const Simian::Vector2& dir, 
                               const Simian::Vector2& min, 
                               const Simian::Vector2& max);

        static bool RayAABB(const Simian::Ray& ray, 
                            const Simian::Vector3& min, 
                            const Simian::Vector3& max);

        static bool RayAABB(const Simian::Ray& ray, 
                            const Simian::Vector3& min, 
                            const Simian::Vector3& max,
                            Simian::Vector3& intersection);
    };
}

#endif
