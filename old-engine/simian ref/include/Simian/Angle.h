/*****************************************************************************/
/*!
\file		Angle.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_ANGLE_H_
#define SIMIAN_ANGLE_H_

#include "SimianPlatform.h"

namespace Simian
{
    /** Represents an Angle, either in radians or in degrees. */
    class SIMIAN_EXPORT Angle
    {
    public:
        /** Gets the angle in radians. */
        virtual f32 Radians() const = 0;
        /** Gets the angle in degrees. */
        virtual f32 Degrees() const = 0;
    };

    class Degree;
    class Radian;

    /** Represents an Angle in degrees. */
    class SIMIAN_EXPORT Degree: public Angle
    {
    private:
        f32 degrees_;
    public:
        /** @see
                Angle::Radians
        */
        f32 Radians() const;
        /** @see
                Angle::Degrees
        */
        f32 Degrees() const;
    public:
        Degree& operator=(Radian& radian);
    public:
        /** Creates an Angle in degrees. */
        Degree(f32 degrees = 0);
        
        /** Creates a Degree from a Radian. */
        Degree(const Radian& radian);
    };

    /** Represents an Angle in radians. */
    class SIMIAN_EXPORT Radian: public Angle
    {
    private:
        f32 radians_;
    public:
        /** @see
                Angle::Radians
        */
        f32 Radians() const;
        /** @see
                Angle::Degrees
        */
        f32 Degrees() const;
    public:
        Radian& operator=(Degree& degree);
    public:
        /** Creates an Angle in radians. */
        Radian(f32 radians_ = 0);

        /** Creates a Radian from a Degree. */
        Radian(const Degree& degree);
    };
}

#endif
