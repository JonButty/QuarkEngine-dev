/*************************************************************************/
/*!
\file		Collision.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Collision.h"

#include <iostream>

namespace Descension
{
    const Simian::f32 Collision::MIN_DIST = 0.001f,
                      Collision::MAX_PENETRATION = 0.001f, //more than pushback
                      Collision::PUSH_BACK = 0.001f, //half of max_penetration
                      Collision::DT_MULTIPLIER = 0.101f; //add 3rd to fix error
    
    //Assuming velocity is already multiplied by *delta time*
    bool Collision::LineCollision( Simian::Vector3 &currPosition,
        Simian::Vector3 &currVelocity,
        const Simian::f32 &radius,
        const Simian::Vector3 &wallPt0,
        const Simian::Vector3 &wallPt1,
        const Simian::Vector3 &wallNormal,
        const bool &response )
    {
        //Step0: test for non-collision (Parallel Direction)
        if (std::fabs(wallNormal.Dot(currVelocity)) <= Simian::Math::EPSILON
            || wallNormal.Dot(currVelocity) > 0.0f)
        {
            return false; //parallel <= epsilon
        }

        //step1: shift line towards ball by radius
        //project ball-point to normal
        Simian::f32 wallShift = radius+MIN_DIST;
        //shift line by radius direction to ball
        Simian::Vector3 shiftPt0 = wallPt0 + (wallShift)*wallNormal;

        //Step2: Find time_hit vs infinite line
        Simian::f32 timeHit = (wallNormal.Dot(shiftPt0) -
            wallNormal.Dot(currPosition)) / wallNormal.Dot(currVelocity);

        //Step3: Find ball_hit position
        Simian::Vector3 interPt = currPosition + timeHit*(currVelocity);
        Simian::Vector3 finalPos = currPosition + currVelocity;

        Simian::f32 sameSide0 = wallNormal.Dot(currPosition);
        Simian::f32 sameSide1 = wallNormal.Dot(finalPos);
        Simian::f32 sameSideCheck = wallNormal.Dot(shiftPt0);

        //Step4: test for non-collision (OUT OF LINE)
        if ((sameSide0 < sameSideCheck && sameSide1 < sameSideCheck)
            || (sameSide0 > sameSideCheck && sameSide1 > sameSideCheck))
        {
                return false; //out of line
        }

        Simian::Vector3 shiftPt1 = wallPt1 + (wallShift)*wallNormal;
        Simian::Vector3 boundCheck0 = interPt - shiftPt0;
        Simian::Vector3 boundCheck1 = shiftPt1 - shiftPt0;
        //Step5: AFTER/BEFORE any side of the finite lines
        if (boundCheck0.Dot(boundCheck1) < 0.0f ||
            boundCheck0.Dot(boundCheck1) >= boundCheck1.Dot(boundCheck1))
        {
                return false; //smaller than starting side
        }

        if (response)
        {   
            if (currVelocity.LengthSquared() < Simian::Math::EPSILON)
                return false;

            LineCollisionResponse(currPosition, interPt, currVelocity, wallNormal);
            currVelocity = currPosition - interPt;
            currPosition = interPt;
        }

        return true; // else, passed test so intersection == true
    }

    //respond by pushback when line contact
    void Collision::LineCollisionResponse( Simian::Vector3 &currPosition,
        const Simian::Vector3 &interPt,
        const Simian::Vector3 &currVelocity,
        const Simian::Vector3 &wallNormal )
    {
        Simian::f32 timeLeft = (1.0f -
                   ((interPt - currPosition).Length() / currVelocity.Length()))
                   * DT_MULTIPLIER;
        Simian::Vector3 finalPos = interPt + MAX_PENETRATION * timeLeft
                        * TileWallCollision::PHYSICS_SCALE * currVelocity;
        currPosition = finalPos + ((wallNormal.Dot(interPt-finalPos)) +
            PUSH_BACK * TileWallCollision::PHYSICS_SCALE) * wallNormal;

        //frame based
        //Simian::Vector3 velVec = currVelocity.Normalized();
        //Simian::Vector3 finalPos = interPt + MAX_PENETRATION
        //    * TileWallCollision::PHYSICS_SCALE * velVec;
        //currPosition = finalPos + ((wallNormal.Dot(interPt-finalPos)) +
        //    PUSH_BACK * TileWallCollision::PHYSICS_SCALE) * wallNormal;
    }

    //--------------------------------------------------------------------------

    //Assuming velocity is already multiplied by *delta time*
    bool Collision::CircleCollision( Simian::Vector3 &currPosition,
        Simian::Vector3 &currVelocity,
        const Simian::f32 &radius,
        const Simian::Vector3 &circleCenter,
        Simian::f32 &getInterTime,
        const bool &response )
    {
        //step0a: get variables and draw triangle with velocity and center
        Simian::Vector3 posDiff = circleCenter - currPosition;
        Simian::f32 sqDist = posDiff.LengthSquared();
        Simian::f32 sqRad = (radius+MIN_DIST*TileWallCollision::PHYSICS_SCALE) *
                            (radius+MIN_DIST*TileWallCollision::PHYSICS_SCALE);

        //overlap push...
        const Simian::f32 pushConst = 10.0f, radTolerance = 0.9f;
        if (response && sqDist < sqRad*radTolerance)
        {
            //--Exact position error /0 checking
            if (sqDist < Simian::Math::EPSILON*TileWallCollision::PHYSICS_SCALE)
            {
                currVelocity += Simian::Vector3(pushConst);
                return true;
            }
            currVelocity += getInterTime*pushConst*(currPosition - circleCenter);
            return true;
        }

        if (currVelocity.LengthSquared() < Simian::Math::EPSILON *
            TileWallCollision::PHYSICS_SCALE)
            return false;

        Simian::Vector3 nVelocity(currVelocity.Normalized());
        Simian::f32 proj_m = nVelocity.Dot(posDiff);

        //step1: opposite direction && outside ball
        //if response is checked, inner check is discarded
        if (sqDist > sqRad && proj_m <= 0.0f) //no checking if spawn inside
            return false;

        //step2: same direction but outside range
        Simian::f32 sqN = sqDist - proj_m * proj_m;
        if (sqN > sqRad)
            return false;

        //step3: calculate return time
        Simian::f32 sqS = sqRad - sqN;

        //safety check denom = 0
        Simian::f32 denom = currVelocity.Length();

        Simian::f32 interTime = getInterTime =
                                (proj_m - Simian::Math::FastSqrt(sqS)) / denom;
        if (interTime > 1.0f || interTime < -1.0f)
        {
            return false;
        }
        //Simian::Vector3 interPt = currPosition + (currVelocity * interTime);
        //Simian::Vector3 finalPos = currPosition + currVelocity;

        if (response)
        {
            Simian::Vector3 initPos(currPosition);
            CircleCollisionResponse(currPosition, currVelocity, getInterTime,
                                  circleCenter, radius+MIN_DIST);
            currVelocity = currPosition - initPos;
            //currPosition = interPt;
        }

        return true;
    }

    //respond by pushback when circle contact
    void Collision::CircleCollisionResponse( Simian::Vector3 &currPosition,
        Simian::Vector3 &currVelocity,
        const Simian::f32 &interTime,
        const Simian::Vector3 &circlePos,
        const Simian::f32 &radius) //radius includes MIN_DIST
    {
        const Simian::f32 constMultiplier = 2.0f;

        Simian::Vector3 interPt = currPosition + interTime * currVelocity;
        //penetrated position
        Simian::Vector3 finalPos(interPt + (MAX_PENETRATION * (1.0f - interTime)
            * DT_MULTIPLIER/constMultiplier * TileWallCollision::PHYSICS_SCALE)
            * currVelocity);

        if ((finalPos - circlePos).LengthSquared() < Simian::Math::EPSILON * 
            TileWallCollision::PHYSICS_SCALE)
        {
            return;
        }

        //from penetrated pos to circle, get normal
        Simian::Vector3 newNormal((finalPos - circlePos).Normalized());

        //this uses 1 less sqrt
        currPosition = circlePos + (radius + PUSH_BACK * constMultiplier *
            TileWallCollision::PHYSICS_SCALE) * newNormal;
    }

    //overloaded response function to handle entity response
    void Collision::CircleCollisionResponse(
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
        const Simian::f32 &interTime)
    {
        //get normalized of pos diff
        Simian::Vector3 nD((interPtA - interPtB).Normalized());
        Simian::f32 Aa= nD.Dot(velA);
        Simian::f32 Ab= nD.Dot(velB);

        //get C prime and numerator for rational expression
        Simian::f32 numer = (2 * (Aa - Ab)) / (radiusA + radiusB);
        Simian::Vector3 reflectedA((velA - (numer * radiusB) * nD).Normalized());
        //Simian::Vector3 reflectedB((velB + (numer * radiusA) * nD).Normalized());

        Simian::Vector3 finalA(interPtA + velA.Dot(circlePos - interPtA) *
                             (1.0f-interTime) * (circlePos - interPtA).Normalized());
        //Simian::Vector3 finalB(interPtB + velB.Dot(circlePos - interPtB) *
        //                      (circlePos - interPtB).Normalized());

        //get ending position
        interPtA = finalA + reflectedA * (PUSH_BACK * 
                    TileWallCollision::PHYSICS_SCALE + (1.0f-interTime));
        //interPtB = finalB + reflectedB * (PUSH_BACK * (1.0f-interTime) *
        //            TileWallCollision::PHYSICS_SCALE);
    }

    bool Collision::PointAABB2D( const Simian::Vector2& p, const Simian::Vector2& min, const Simian::Vector2& max )
    {
        if (p.X() > min.X() && 
            p.Y() > min.Y() &&
            p.X() < max.X() && 
            p.Y() < max.Y())
            return true;
        return false;
    }

    bool Collision::LineAABB2D( const Simian::Vector2& p, const Simian::Vector2& dir, const Simian::Vector2& min, const Simian::Vector2& max )
    {
        // check if line is a point
        if (dir.LengthSquared() < Simian::Math::EPSILON)
            return PointAABB2D(p, min, max);

        // get ortho dir
        Simian::Vector2 orthoDir(dir.Y(), -dir.X());

        Simian::Vector2 pts[] = {
            Simian::Vector2(min.X(), min.Y()),
            Simian::Vector2(max.X(), min.Y()),
            Simian::Vector2(max.X(), max.Y()),
            Simian::Vector2(min.X(), max.Y())
        };

        float mind = 0, maxd = 0;
        for (int i = 0; i < 4; ++i)
        {
            float dot = orthoDir.Dot(pts[i] - p);
            maxd = dot > maxd ? dot : maxd;
            mind = dot < mind ? dot : mind;
        }

        if (maxd > 0 && mind < 0)
            return true;
        return false;
    }

    bool Collision::RayAABB( const Simian::Ray& ray, const Simian::Vector3& min, const Simian::Vector3& max )
    {
        // 3 slab test (christer ericsson)
        if (LineAABB2D(Simian::Vector2(ray.Position().X(), ray.Position().Y()),
                       Simian::Vector2(ray.Direction().X(), ray.Direction().Y()),
                       Simian::Vector2(min.X(), min.Y()),
                       Simian::Vector2(max.X(), max.Y())) &&
            LineAABB2D(Simian::Vector2(ray.Position().X(), ray.Position().Z()),
                       Simian::Vector2(ray.Direction().X(), ray.Direction().Z()),
                       Simian::Vector2(min.X(), min.Z()),
                       Simian::Vector2(max.X(), max.Z())) &&
            LineAABB2D(Simian::Vector2(ray.Position().Y(), ray.Position().Z()),
                       Simian::Vector2(ray.Direction().Y(), ray.Direction().Z()),
                       Simian::Vector2(min.Y(), min.Z()),
                       Simian::Vector2(max.Y(), max.Z())))
            return true;

        return false;
    }

    bool Collision::RayAABB( const Simian::Ray& ray, const Simian::Vector3& min, const Simian::Vector3& max, Simian::Vector3& intersection )
    {
        // RAY AABB FROM COLLISION DETECTION BOOK
        Simian::f32 tmin = 0;
        Simian::f32 tmax = FLT_MAX;

        for (Simian::u32 i = 0; i < 3; ++i)
        {
            // if the direction on the axis is straight (becomes a 2d problem)
            if (std::abs(ray.Direction()[i]) < Simian::Math::EPSILON)
            {
                // this means the ray is parallel and straight outside the aabb.. reject!
                if (ray.Position()[i] < min[i] || ray.Position()[i] > max[i]) 
                    return false;
            }
            else
            {
                float denom = 1.0f/ray.Direction()[i];
                float t0 = (min[i] - ray.Position()[i]) * denom;
                float t1 = (max[i] - ray.Position()[i]) * denom;
                if (t0 > t1) std::swap(t0, t1);

                // update min and max
                if (t0 > tmin) tmin = t0;
                if (t1 < tmax) tmax = t1;

                if (tmin > tmax) return false;
            }
        }

        intersection = ray.Position() + ray.Direction() * tmin;
        return true;
    }
}
