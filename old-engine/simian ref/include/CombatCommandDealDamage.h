/*************************************************************************/
/*!
\file		CombatCommandDealDamage.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_COMBATCOMMANDDEALDAMAGE_H_
#define SIMIAN_COMBATCOMMANDDEALDAMAGE_H_

#include "CombatCommandSystem.h"
#include "Simian/Delegate.h"

namespace Descension
{
    class CombatCommandDealDamage: public CombatCommand
    {
    private:
        Simian::s32 relativeDamage_;
    public:
        CombatCommandDealDamage();
        void Execute();
        static CombatCommandDealDamage* Create(Simian::s32 damage);
    };
}

#endif
