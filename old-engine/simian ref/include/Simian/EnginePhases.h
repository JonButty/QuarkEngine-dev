/************************************************************************/
/*!
\file		EnginePhases.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_ENGINEPHASES_H_
#define SIMIAN_ENGINEPHASES_H_

namespace Simian
{
    enum EnginePhases
    {
        P_TRANSFORM,
        P_POSTTRANSFORM,
        P_UPDATE,
        P_PHYSICS_RESET,
        P_PHYSICS,
        P_POSTPHYSICS,
        P_RENDER,
        P_TOTAL
    };
}

#endif
