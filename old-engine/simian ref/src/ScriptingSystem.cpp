/*************************************************************************/
/*!
\file		ScriptingSystem.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "ScriptingSystem.h"
#include "ScriptFile.h"

#include "Simian/SimianTypes.h"

#include <iostream>

namespace Descension
{
    static const Simian::u32 INITIAL_STACK = 1024;

    ScriptingSystem::ScriptingSystem()
        : squirrelVm_(0)
    {
    }

    //--------------------------------------------------------------------------

    void ScriptingSystem::Init()
    {
        squirrelVm_ = sq_open(INITIAL_STACK);
    }

    void ScriptingSystem::Deinit()
    {
        sq_close(squirrelVm_);
    }
}
