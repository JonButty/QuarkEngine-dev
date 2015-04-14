/*************************************************************************/
/*!
\file		CombatCommandSystem.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "CombatCommandSystem.h"

namespace Descension
{
    void CombatCommandSystem::AddCommand( GCAttributes* source, GCAttributes* target, CombatCommand* command )
    {
        command->source_ = source;
        command->target_ = target;
        commands_.push_back(command);
    }

    void CombatCommandSystem::Process()
    {
        // execute the commands
        for (Simian::u32 i = 0; i < commands_.size(); ++i)
            commands_[i]->Execute();
        // clear all commands and the stack alloc
        Clear();
    }

    void CombatCommandSystem::Clear()
    {
        commands_.clear();
        alloc_.Clear();
    }
}
