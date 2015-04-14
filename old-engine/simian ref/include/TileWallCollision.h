/*************************************************************************/
/*!
\file		TileWallCollision.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_TILEWALLCOLLISION_H_
#define DESCENSION_TILEWALLCOLLISION_H_

#include "Simian/Vector3.h"
#include "Simian/SimianTypes.h"
#include "Simian/Math.h"
#include "TileMap.h"
#include "Collision.h"

namespace Descension
{
    class TileWallCollision
    {
    public:
        static const Simian::u32 NUM_WALLS;
        //solves precision issues
        static const Simian::f32 PHYSICS_SCALE, DWALL_MULTIPLIER,
                                 ACUTE_PADDING, ACUTE_IPADDING;
    public:
        static bool CheckTileCollision(Simian::Vector3 &currPosition,
            const Simian::Vector3 &currVelocity,
            const Simian::f32 &radius,
            const TileMap *tileMap,
            const bool &response = true,
            const bool &collideWithVoid = true); //default = set position aft. collide

        static bool inline IterativeCheck(Simian::Vector3 &wallVector,
            Simian::Vector3 &wallNormal,
            Simian::Vector3 &wallPt,
            Simian::Vector3 &position,
            Simian::Vector3 &velocity,
            Simian::Vector3 &currPosition, //real original reference position
            const TileMap *tileMap,
            const Simian::u32 &posX,
            const Simian::u32 &posZ,
            const Simian::f32 &radius,
            const bool &response,
            const bool &collisionFlag,
            const bool &collideWithVoid,
            bool &nextGridCheck);

        //helper to check diagonal wall for projectiles
        static inline bool CheckDiagonal(bool collideVoid, const TileMap* tileMap,
            Simian::u32 diagFlag, Simian::u32 x, Simian::u32 z);
        static inline bool CheckAcute(Simian::f32 radius, const TileMap* tileMap,
            Simian::u32 diagFlag, Simian::u32 x, Simian::u32 z,
            Simian::Vector3& position, Simian::Vector3& velocity, bool response);
        static inline bool NearestWallCheck(const Simian::Vector3 currPosition,
                                     Simian::f32 radius, Simian::u32 posX,
                                     Simian::u32 posZ);
    };
}

#endif
