/********************************************************************
	created:	2015/01/27
	filename: 	mathTest.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef MATHTEST_H
#define MATHTEST_H

#include "math/vector2.h"

class MathTest
{
public:

    template <typename T>
    static bool Vector2Test()
    {
        QuarkVec2D vec;
        vec.X();
        return true;
    }

private:

    template <typename T>
    static bool vectorInitValue()
    {
        
    }
};

#endif