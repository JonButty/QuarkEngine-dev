/*************************************************************************/
/*!
\file		ScriptingSystem.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_SCRIPTINGSYSTEM_H_
#define DESCENSION_SCRIPTINGSYSTEM_H_

#include "Simian/Singleton.h"
#include "Simian/DataLocation.h"
#include "Simian/Delegate.h"

#include "squirrel.h"

namespace Descension
{
    class ScriptingSystem: public Simian::Singleton<ScriptingSystem>
    {
    private:
        HSQUIRRELVM squirrelVm_;
    public:
        ScriptingSystem();

        void Init();
        void Deinit();

        template <typename T>
        friend class ScriptFile;
    };
}

#endif
