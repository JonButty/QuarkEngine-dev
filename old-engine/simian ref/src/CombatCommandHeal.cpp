/*************************************************************************/
/*!
\file		CombatCommandHeal.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "CombatCommandHeal.h"

namespace Descension
{
    CombatCommandHeal::CombatCommandHeal()
        : healthIncrement_(0),
          manaIncrement_(0)
    {
    }

    //--------------------------------------------------------------------------

    void CombatCommandHeal::Execute()
    {
        target_->Health(target_->Health() + healthIncrement_);
        target_->Mana(target_->Mana() + manaIncrement_);
    }

    CombatCommandHeal* CombatCommandHeal::Create( Simian::s32 healthIncrement, Simian::f32 manaIncrement )
    {
        CombatCommandHeal* command;
        CombatCommandSystem::Instance().CreateCommand(command);
        command->healthIncrement_ = healthIncrement;
        command->manaIncrement_ = manaIncrement;
        return command;
    }
}
