/*************************************************************************/
/*!
\file		CombatCommandDealDamage.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "CombatCommandDealDamage.h"

namespace Descension
{
    CombatCommandDealDamage::CombatCommandDealDamage()
        : relativeDamage_(0)
    {
    }

    //--------------------------------------------------------------------------

    void CombatCommandDealDamage::Execute()
    {
        target_->DealDamage(relativeDamage_, source_);
    }

    CombatCommandDealDamage* CombatCommandDealDamage::Create( Simian::s32 damage )
    {
        CombatCommandDealDamage* ret;
        CombatCommandSystem::Instance().CreateCommand(ret);
        ret->relativeDamage_ = damage;
        return ret;
    }
}
