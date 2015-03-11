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
#include "TileWallCollision.h"
#include "EntityCollision.h"

#include "Simian/DebugRenderer.h"

#include <iostream>

namespace Descension
{
    const Simian::u32 TileWallCollision::NUM_WALLS = Tile::TILE_FLAG_TOTAL - 1;
    const Simian::f32 TileWallCollision::PHYSICS_SCALE = 10000.0f,
                      TileWallCollision::DWALL_MULTIPLIER = 1.001f, //1.001f
                      TileWallCollision::ACUTE_PADDING = 0.25f,
                      TileWallCollision::ACUTE_IPADDING = 1.0f - ACUTE_PADDING;

    bool TileWallCollision::CheckTileCollision( Simian::Vector3 &currPosition,
                                const Simian::Vector3 &currVelocity,
                                const Simian::f32 &radius,
                                const TileMap *tileMap,
                                const bool &response,
                                const bool &collideWithVoid )
    {
        if (currVelocity.LengthSquared() < Simian::Math::EPSILON)
            return false;

        Simian::Vector3 velocity(currVelocity), position(currPosition);

        //position in tile space
        Simian::f32 negatorX = currVelocity.X() < 0.0f ? -1.0f : 1.0f;
        Simian::f32 negatorZ = currVelocity.Z() < 0.0f ? -1.0f : 1.0f;
        Simian::s32 posX = static_cast<Simian::s32>(currPosition.X() +
                        (radius + Collision::MIN_DIST) * (negatorX*-1.0f));
        Simian::s32 posZ = static_cast<Simian::s32>(currPosition.Z() +
                        (radius + Collision::MIN_DIST) * (negatorZ*-1.0f));
        Simian::s32 endX = static_cast<Simian::s32>(currPosition.X() +
                  currVelocity.X() + (radius + Collision::MIN_DIST) * negatorX);
        Simian::s32 endZ = static_cast<Simian::s32>(currPosition.Z() +
                  currVelocity.Z() + (radius + Collision::MIN_DIST) * negatorZ);

        posX /= static_cast<Simian::s32>(PHYSICS_SCALE);
        posZ /= static_cast<Simian::s32>(PHYSICS_SCALE);
        endX /= static_cast<Simian::s32>(PHYSICS_SCALE);
        endZ /= static_cast<Simian::s32>(PHYSICS_SCALE);

        //restrictions to map boundaries
        posX = posX < 0 ? 0 : Simian::Math::Min(posX,
            static_cast<Simian::s32>(tileMap->Width() - 1));
        posZ = posZ < 0 ? 0 : Simian::Math::Min(posZ,
            static_cast<Simian::s32>(tileMap->Height() - 1));
        endX = endX < 0 ? 0 : Simian::Math::Min(endX,
            static_cast<Simian::s32>(tileMap->Width() - 1));
        endZ = endZ < 0 ? 0 : Simian::Math::Min(endZ,
            static_cast<Simian::s32>(tileMap->Height() - 1));

        bool collisionFlag = false;

        Simian::Vector3 wallVector(1.0f * PHYSICS_SCALE, 0.0f, 0.0f); //normalized
        Simian::Vector3 wallNormal(0.0f, 0.0f, 1.0f); //normalized

        Simian::u32 xIteration = std::abs(endX - posX) + 1;
        Simian::u32 zIteration = std::abs(endZ - posZ) + 1;

        //new flag for grid to grid check
        bool nextGridCheck = false;

        for (Simian::u32 i = 0; i < xIteration; ++i)
        {
            endX = posX + static_cast<Simian::s32>(negatorX) * i;
            for (Simian::u32 j = 0; j < zIteration; ++j)
            {
                endZ = posZ + static_cast<Simian::s32>(negatorZ) * j;

                Simian::Vector3 wallPt(static_cast<Simian::f32>(endX) * PHYSICS_SCALE, 0.0f,
                    static_cast<Simian::f32>(endZ) * PHYSICS_SCALE); //for wallPt0

                DebugRendererDrawCubeEx(Simian::Vector3((endX+0.5f), 0.05f, (endZ+0.5f)),
                    Simian::Vector3(0.8f, 0.1f, 0.8f),
                    Simian::Color(0.0, 1.0, 1.0),
                    Simian::Degree(0), Simian::Vector3::UnitY);

                Simian::u32 loopCount = 0;
                const Simian::u32 maxLoop = 32;
                while (TileWallCollision::IterativeCheck(wallVector,
                    wallNormal,
                    wallPt,
                    position,
                    velocity,
                    currPosition, //real original reference position
                    tileMap,
                    endX,
                    endZ,
                    radius,
                    response,
                    collisionFlag,
                    collideWithVoid,
                    nextGridCheck)
                    && ((++loopCount) <= maxLoop))
                {
                    collisionFlag = true;
                    if (!response)
                    {
                        i = xIteration, j = zIteration;
                        break;
                    }
                }
            }
        }

        //this is the final position after collision check
        //if hit, this is updated
        currPosition = position + velocity;
        return collisionFlag;
    }

    bool TileWallCollision::IterativeCheck(Simian::Vector3 &wallVector,
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
            bool &nextGridCheck)
    {
        //from EAST, CCW rotation
        bool loopAgain = false;

        //----------------------------------------------------------------------
        //diagonal wall
        if (SIsFlagSet((*tileMap)[posX][posZ].Flag, Tile::TILE_DIAGUP) &&
            CheckDiagonal(collideWithVoid, tileMap, Tile::TILE_DIAGUP, posX, posZ) )
        {
            Simian::Vector3 pt0(wallPt);
            Simian::Vector3 pt1(wallPt + wallVector + wallNormal * PHYSICS_SCALE);
            Simian::Vector3 dNormal(pt1 - pt0);
            dNormal = Simian::Vector3(-dNormal.Z(), dNormal.Y(), dNormal.X()).Normalized();

            //scaled by physics scale
            Simian::f32 newRad = Simian::Math::FastSqrt(2 * (radius * radius));
            Simian::f32 dLine = dNormal.Dot(position - pt0);
            //Simian::f32 pbMultiplier = velocity.Length()/9;

            //check sideUp vs sideDown
            if (dLine < 0.0f)
                dNormal = -dNormal;

            if (((std::fabs(dLine) < newRad && collisionFlag) &&
                ((position-pt0).Dot(pt1 - pt0) > 0.0f &&
                (position-pt1).Dot(pt0 - pt1) > 0.0f)) || nextGridCheck)
            {
                velocity = position - currPosition;
                position = currPosition;
                //check for overlapping
                Simian::u32 loopCount = 0;
                const Simian::u32 maxLoop = 64;
                //resolves overlap on loop
                while ((std::fabs(dNormal.Dot(position - pt0)) <= newRad)
                    && ((++loopCount) <= maxLoop))
                {
                    position += Collision::PUSH_BACK// * pbMultiplier)
                                * dNormal * PHYSICS_SCALE;
                }
                //std::cout << "diagonal penetration " <<std::endl;
                loopAgain = true;
                nextGridCheck = true;
            }

            if (Collision::LineCollision(position,
                velocity, //object velocity
                newRad, //object radius
                pt0, //wallPt0
                pt1, //wallPt1
                //wallVector, //normalized
                dNormal*DWALL_MULTIPLIER,
                response)) //normalized
            {
                loopAgain = true;
                //std::cout << "diagonal Hit" << i << std::endl;
            }
        }

        if (SIsFlagSet((*tileMap)[posX][posZ].Flag, Tile::TILE_DIAGDOWN) &&
            CheckDiagonal(collideWithVoid, tileMap, Tile::TILE_DIAGDOWN, posX, posZ) )
        {
            Simian::Vector3 pt0(wallPt + PHYSICS_SCALE * wallNormal);
            Simian::Vector3 pt1(wallPt + wallVector);
            Simian::Vector3 dNormal(pt1 - pt0);
            dNormal = Simian::Vector3(-dNormal.Z(), dNormal.Y(), dNormal.X()).Normalized();

            //scaled by physics scale
            Simian::f32 newRad = Simian::Math::FastSqrt(2 * (radius * radius));
            Simian::f32 dLine = dNormal.Dot(position - pt0);
            //Simian::f32 pbMultiplier = velocity.Length()/9;

            //check sideUp vs sideDown
            if (dLine < 0.0f)
                dNormal = -dNormal;

            if (((std::fabs(dLine) < newRad && collisionFlag) &&
                ((position-pt0).Dot(pt1 - pt0) > 0.0f &&
                (position-pt1).Dot(pt0 - pt1) > 0.0f)) || nextGridCheck)
            {
                velocity = position - currPosition;
                position = currPosition;
                //check for overlapping
                Simian::u32 loopCount = 0;
                const Simian::u32 maxLoop = 64;
                //resolves overlap on loop
                while ((std::fabs(dNormal.Dot(position - pt0)) <= newRad)
                    && ((++loopCount) <= maxLoop))
                {
                    position += Collision::PUSH_BACK// * pbMultiplier)
                                * dNormal * PHYSICS_SCALE;
                }
                //std::cout << "diagonal penetration " <<std::endl;
                loopAgain = true;
                nextGridCheck = true;
            }

            if (Collision::LineCollision(position,
                velocity, //object velocity
                newRad, //object radius
                pt0, //wallPt0
                pt1, //wallPt1
                //wallVector, //normalized
                dNormal*DWALL_MULTIPLIER,
                response)) //normalized
            {
                loopAgain = true;
                //std::cout << "diagonal Hit" << i << std::endl;
            }
        }

        //----------------------------------------------------------------------
        //first 4 walls
        for (Simian::u32 i=0; i<4; ++i)
        {
            //rotate the vector for the loop
            wallVector = Simian::Vector3(-wallVector.Z(), wallVector.Y(), wallVector.X());
            wallNormal = Simian::Vector3(-wallNormal.Z(), wallNormal.Y(), wallNormal.X());
            wallPt += -wallNormal * PHYSICS_SCALE; //this is wallPt0

            Simian::s32 xStep = static_cast<int>(posX-wallNormal.X());
            Simian::s32 zStep = static_cast<int>(posZ-wallNormal.Z());
            xStep = xStep < 0 ? 0 : Simian::Math::Min(xStep,
                static_cast<Simian::s32>(tileMap->Width() - 1));
            zStep = zStep < 0 ? 0 : Simian::Math::Min(zStep,
                static_cast<Simian::s32>(tileMap->Height() - 1));

            //check if tile is valid and existent
            if (SIsFlagSet((*tileMap)[posX][posZ].Flag, Tile::TILE_EAST+i) ||
                (collideWithVoid &&
                !SIsFlagSet((*tileMap)[xStep][zStep].Flag, Tile::TILE_FLOOR)) )
            {
                //if current position is too close to a wall
                if ((std::fabs(wallNormal.Dot(position - wallPt)) < radius
                    )/*&& collisionFlag) */&& //--was enabled before
                    NearestWallCheck(currPosition, radius, posX, posZ))
                {
                    velocity = position - currPosition;
                    position = currPosition;
                    //check for overlapping
                    Simian::u32 loopCount = 0;
                    const Simian::u32 maxLoop = 10;
                    //resolves overlap on loop
                    while ((std::fabs(wallNormal.Dot(position - wallPt)) < radius)
                        && ((++loopCount) <= maxLoop))
                    {
                        position += Collision::PUSH_BACK * wallNormal * PHYSICS_SCALE;
                    }
                    //std::cout << "WallTooClose: " << i << std::endl;
                    loopAgain = true;
                }

                Simian::Vector3 wallPt0 = wallPt - (radius) * wallVector;
                Simian::Vector3 wallPt1 = wallPt + (radius + 1.0f) * wallVector;
                //check#1 line
                if (Collision::LineCollision(position,
                    velocity, //object velocity
                    radius, //object radius
                    wallPt0, //wallPt0
                    wallPt1, //wallPt1
                    wallNormal,
                    response)) //normalized
                {
                    loopAgain = true;
                    //std::cout << "WallHit: " << i << std::endl;
                    continue;
                }

                //LINE CHECK ON SIDE
                Simian::Vector3 shiftPositive = (radius) * wallNormal;
                Simian::Vector3 sideNormal(-wallNormal.Z(), wallNormal.Y(),
                                            wallNormal.X());

                //check#1 line
                if (Collision::LineCollision(position,
                    velocity, //object velocity
                    radius, //object radius
                    wallPt - shiftPositive, //wallPt0
                    wallPt + shiftPositive, //wallPt1
                    //wallVector, //normalized
                    sideNormal, response)) //normalized
                {
                    loopAgain = true;
                    //std::cout << "sideHit" << i << std::endl;
                    continue;
                }

                //check#2 line
                if (Collision::LineCollision(position,
                    velocity, //object velocity
                    radius, //object radius
                    wallPt + wallVector + shiftPositive, //wallPt0
                    wallPt + wallVector - shiftPositive, //wallPt1
                    //wallVector, //normalized
                    -sideNormal, response)) //normalized
                {
                    loopAgain = true;
                    //std::cout << "sideHit" << i << std::endl;
                    continue;
                }
            }
        }
        //if there's collision, loop until no more left
        return loopAgain;
    }

    //Helper check: diagonal to void for projectiles
    bool TileWallCollision::CheckDiagonal(bool collideVoid, const TileMap* tileMap,
                            Simian::u32 diagFlag, Simian::u32 x, Simian::u32 z)
    {
        if (collideVoid)
            return true;

        Simian::u32 east = x+1 >= tileMap->Width() ? tileMap->Width() : x+1;
        Simian::u32 north = z+1 >= tileMap->Height() ? tileMap->Height() : z+1;
        Simian::u32 west = x == 0 ? 0 : x-1;
        Simian::u32 south = z == 0 ? 0 : z-1;

        //first filter: 0 or max
        if (east == tileMap->Width() || north == tileMap->Height() ||
            west == 0 || south == 0)
            return false; //WILL NOT CHECK

        if (diagFlag == Tile::TILE_DIAGUP || diagFlag == Tile::TILE_DIAGDOWN)
        {
            Simian::u32 count = 0;

            count += !SIsFlagSet((*tileMap)[x][north].Flag, Tile::TILE_FLOOR);
            count += !SIsFlagSet((*tileMap)[x][south].Flag, Tile::TILE_FLOOR);
            count += !SIsFlagSet((*tileMap)[east][z].Flag, Tile::TILE_FLOOR);
            count += !SIsFlagSet((*tileMap)[west][z].Flag, Tile::TILE_FLOOR);

            if (count > 1)
                return false; //WILL NOT CHECK
        }

        return true;
    }

    bool TileWallCollision::CheckAcute(Simian::f32 radius, const TileMap* tileMap,
        Simian::u32 diagFlag, Simian::u32 x, Simian::u32 z,
        Simian::Vector3& position, Simian::Vector3& velocity, bool response)
    {
        const Simian::Vector3 northVec(0, 0, 1.0f),
                              eastVec(1.0f, 0, 0);
        const Simian::f32 radMult = 1.9625f;

        Simian::u32 east = x+1 >= tileMap->Width() ? tileMap->Width() : x+1;
        Simian::u32 north = z+1 >= tileMap->Height() ? tileMap->Height() : z+1;
        Simian::s32 west = x-1;
        Simian::u32 south = z-1;
        Simian::f32 getInterTime = 0.0f;

        if (diagFlag == Tile::TILE_DIAGUP)
        {
            //--EAST
            if (east < tileMap->Width() &&
               (SIsFlagSet((*tileMap)[east][z].Flag, Tile::TILE_DIAGDOWN) ||
               SIsFlagSet((*tileMap)[x][z].Flag, Tile::TILE_EAST) ))
            {
                if (Collision::CircleCollision( position,
                    velocity, radius*radMult,
                    Simian::Vector3(east*PHYSICS_SCALE, 0, north*PHYSICS_SCALE),
                    getInterTime, response))
                    return true;

                DebugRendererDrawCircle(
                    Simian::Vector3(east*1.0f, radius*radMult / PHYSICS_SCALE, north*1.0f),
                    radius*radMult / PHYSICS_SCALE,
                    Simian::Color(1.0f, 0.25f, .0f));

                //if (Collision::LineCollision(position,
                //    velocity, //object velocity
                //    radius, //object radius
                //    Simian::Vector3(x+ACUTE_IPADDING, 0.0f, z+ACUTE_IPADDING) * PHYSICS_SCALE,
                //    Simian::Vector3(x+ACUTE_IPADDING+2*ACUTE_PADDING, 0.0f,
                //                    z+ACUTE_IPADDING) * PHYSICS_SCALE, //wallPt1
                //    -northVec,
                //    response)) //normalized
                //{
                //    return true;
                //}
            }
            //--NORTH
            if (north < tileMap->Height() &&
                (SIsFlagSet((*tileMap)[x][north].Flag, Tile::TILE_DIAGDOWN) ||
                SIsFlagSet((*tileMap)[x][z].Flag, Tile::TILE_NORTH) ))
            {
                if (Collision::CircleCollision( position,
                    velocity, radius*radMult,
                    Simian::Vector3(east*PHYSICS_SCALE, 0, north*PHYSICS_SCALE),
                    getInterTime, response))
                    return true;

                DebugRendererDrawCircle(
                    Simian::Vector3(east*1.0f, radius*radMult / PHYSICS_SCALE, north*1.0f),
                    radius*radMult / PHYSICS_SCALE,
                    Simian::Color(1.0f, 0.25f, .0f));

                //if (Collision::LineCollision(position,
                //    velocity, //object velocity
                //    radius, //object radius
                //    Simian::Vector3(x+ACUTE_IPADDING, 0.0f, z+ACUTE_IPADDING) * PHYSICS_SCALE,
                //    Simian::Vector3(x+ACUTE_IPADDING, 0.0f,
                //                    z+ACUTE_IPADDING+2*ACUTE_PADDING)  * PHYSICS_SCALE, //wallPt1
                //    -eastVec,
                //    response)) //normalized
                //{
                //    return true;
                //}
            }
            //--WEST
            if (west >= 0 &&
                (SIsFlagSet((*tileMap)[west][z].Flag, Tile::TILE_DIAGDOWN) ||
                SIsFlagSet((*tileMap)[x][z].Flag, Tile::TILE_WEST) ))
            {
                if (Collision::CircleCollision( position,
                    velocity, radius*radMult,
                    Simian::Vector3(x*PHYSICS_SCALE, 0, z*PHYSICS_SCALE),
                    getInterTime, response))
                    return true;

                DebugRendererDrawCircle(
                    Simian::Vector3(x*1.0f, radius*radMult / PHYSICS_SCALE, z*1.0f),
                    radius*radMult / PHYSICS_SCALE,
                    Simian::Color(1.0f, 0.25f, .0f));

                //if (Collision::LineCollision(position,
                //    velocity, //object velocity
                //    radius, //object radius
                //    Simian::Vector3(x+ACUTE_PADDING, 0.0f, z+ACUTE_PADDING) * PHYSICS_SCALE,
                //    Simian::Vector3(x-ACUTE_PADDING, 0.0f, z+ACUTE_PADDING) * PHYSICS_SCALE, //wallPt1
                //    northVec,
                //    response)) //normalized
                //{
                //    return true;
                //}
            }
            //--SOUTH
            if (south >= 0 &&
                (SIsFlagSet((*tileMap)[x][south].Flag, Tile::TILE_DIAGDOWN) ||
                SIsFlagSet((*tileMap)[x][z].Flag, Tile::TILE_SOUTH) ))
            {
                if (Collision::CircleCollision( position,
                    velocity, radius*radMult,
                    Simian::Vector3(x*PHYSICS_SCALE, 0, z*PHYSICS_SCALE),
                    getInterTime, response))
                    return true;

                DebugRendererDrawCircle(
                    Simian::Vector3(x*1.0f, radius*radMult / PHYSICS_SCALE, z*1.0f),
                    radius*radMult / PHYSICS_SCALE,
                    Simian::Color(1.0f, 0.25f, .0f));

                //if (Collision::LineCollision(position,
                //    velocity, //object velocity
                //    radius, //object radius
                //    Simian::Vector3(x+ACUTE_PADDING, 0.0f, z+ACUTE_PADDING) * PHYSICS_SCALE,
                //    Simian::Vector3(x+ACUTE_PADDING, 0.0f, z-ACUTE_PADDING) * PHYSICS_SCALE, //wallPt1
                //    eastVec,
                //    response)) //normalized
                //{
                //    return true;
                //}
            }
            return false;
        }
        if (diagFlag == Tile::TILE_DIAGDOWN)
        {
            //--EAST
            if (east < tileMap->Width() &&
                (SIsFlagSet((*tileMap)[east][z].Flag, Tile::TILE_DIAGUP) ||
                SIsFlagSet((*tileMap)[x][z].Flag, Tile::TILE_EAST) ))
            {
                if (Collision::CircleCollision( position,
                    velocity, radius*radMult,
                    Simian::Vector3(east*PHYSICS_SCALE, 0, z*PHYSICS_SCALE),
                    getInterTime, response))
                    return true;

                DebugRendererDrawCircle(
                    Simian::Vector3(east*1.0f, radius*radMult / PHYSICS_SCALE, z*1.0f),
                    radius*radMult / PHYSICS_SCALE,
                    Simian::Color(1.0f, 0.25f, .0f));

                //if (Collision::LineCollision(position,
                //    velocity, //object velocity
                //    radius, //object radius
                //    Simian::Vector3(x+ACUTE_IPADDING, 0.0f, z+ACUTE_IPADDING) * PHYSICS_SCALE,
                //    Simian::Vector3(x+ACUTE_IPADDING+2*ACUTE_PADDING, 0.0f,
                //    z+ACUTE_IPADDING) * PHYSICS_SCALE, //wallPt1
                //    northVec,
                //    response)) //normalized
                //{
                //    return true;
                //}
            }
            //--NORTH
            if (north < tileMap->Height() &&
                (SIsFlagSet((*tileMap)[x][north].Flag, Tile::TILE_DIAGUP) ||
                SIsFlagSet((*tileMap)[x][z].Flag, Tile::TILE_NORTH) ))
            {
                if (Collision::CircleCollision( position,
                    velocity, radius*radMult,
                    Simian::Vector3(x*PHYSICS_SCALE, 0, north*PHYSICS_SCALE),
                    getInterTime, response))
                    return true;

                DebugRendererDrawCircle(
                    Simian::Vector3(x*1.0f, radius*radMult / PHYSICS_SCALE, north*1.0f),
                    radius*radMult / PHYSICS_SCALE,
                    Simian::Color(1.0f, 0.25f, .0f));

                //if (Collision::LineCollision(position,
                //    velocity, //object velocity
                //    radius, //object radius
                //    Simian::Vector3(x+ACUTE_PADDING, 0.0f, z+ACUTE_IPADDING) * PHYSICS_SCALE,
                //    Simian::Vector3(x+ACUTE_PADDING, 0.0f,
                //    z+ACUTE_IPADDING+2*ACUTE_PADDING) * PHYSICS_SCALE, //wallPt1
                //    eastVec,
                //    response)) //normalized
                //{
                //    return true;
                //}
            }
            //--WEST
            if (west >= 0 &&
                (SIsFlagSet((*tileMap)[west][z].Flag, Tile::TILE_DIAGUP) ||
                SIsFlagSet((*tileMap)[x][z].Flag, Tile::TILE_WEST) ))
            {
                if (Collision::CircleCollision( position,
                    velocity, radius*radMult,
                    Simian::Vector3(x*PHYSICS_SCALE, 0, north*PHYSICS_SCALE),
                    getInterTime, response))
                    return true;

                DebugRendererDrawCircle(
                    Simian::Vector3(x*1.0f, radius*radMult / PHYSICS_SCALE, north*1.0f),
                    radius*radMult / PHYSICS_SCALE,
                    Simian::Color(1.0f, 0.25f, .0f));

                //if (Collision::LineCollision(position,
                //    velocity, //object velocity
                //    radius, //object radius
                //    Simian::Vector3(x+ACUTE_PADDING, 0.0f, z+ACUTE_IPADDING) * PHYSICS_SCALE,
                //    Simian::Vector3(x-ACUTE_PADDING, 0.0f, z+ACUTE_IPADDING) * PHYSICS_SCALE, //wallPt1
                //    -northVec,
                //    response)) //normalized
                //{
                //    return true;
                //}
            }
            //--SOUTH
            if (south >= 0 &&
                (SIsFlagSet((*tileMap)[x][south].Flag, Tile::TILE_DIAGUP) ||
                SIsFlagSet((*tileMap)[x][z].Flag, Tile::TILE_SOUTH) ))
            {
                if (Collision::CircleCollision( position,
                    velocity, radius*radMult,
                    Simian::Vector3(east*PHYSICS_SCALE, 0, z*PHYSICS_SCALE),
                    getInterTime, response))
                    return true;

                DebugRendererDrawCircle(
                    Simian::Vector3(east*1.0f, radius*radMult / PHYSICS_SCALE, z*1.0f),
                    radius*radMult / PHYSICS_SCALE,
                    Simian::Color(1.0f, 0.25f, .0f));

                //if (Collision::LineCollision(position,
                //    velocity, //object velocity
                //    radius, //object radius
                //    Simian::Vector3(x+ACUTE_IPADDING, 0.0f, z+ACUTE_PADDING) * PHYSICS_SCALE,
                //    Simian::Vector3(x+ACUTE_IPADDING, 0.0f, z-ACUTE_PADDING) * PHYSICS_SCALE, //wallPt1
                //    -eastVec,
                //    response)) //normalized
                //{
                //    return true;
                //}
            }
        }
        return false;
    }

    bool TileWallCollision::NearestWallCheck(const Simian::Vector3 currPosition,
        Simian::f32 radius, Simian::u32 posX, Simian::u32 posZ)
    {
        return (static_cast<Simian::u32>((currPosition.X() + radius) / PHYSICS_SCALE)
                == posX &&
               (static_cast<Simian::u32>((currPosition.Z() + radius) / PHYSICS_SCALE)
                == posZ) ) ||
               (static_cast<Simian::u32>((currPosition.X() - radius) / PHYSICS_SCALE)
                == posX &&
               (static_cast<Simian::u32>((currPosition.Z() - radius) / PHYSICS_SCALE)
                == posZ) );

        //return (
        //    static_cast<Simian::u32>((currPosition.X() + radius) / PHYSICS_SCALE)
        //    == posX &&
        //    static_cast<Simian::u32>((currPosition.Z() + radius) / PHYSICS_SCALE)
        //    == posZ) ;
    }
}
