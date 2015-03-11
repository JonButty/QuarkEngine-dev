/*************************************************************************/
/*!
\file		SpawnFactory.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "SpawnFactory.h"
#include "SpawnEventTypes.h"

namespace Descension
{
    SpawnFactory::SpawnFactory()
        : Factory<SpawnEvent>(SE_TOTAL)
    {
    }
}