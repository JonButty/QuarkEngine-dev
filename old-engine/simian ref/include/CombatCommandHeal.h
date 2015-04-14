/*************************************************************************/
/*!
\file		CombatCommandHeal.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_COMBATCOMMANDHEAL_H_
#define DESCENSION_COMBATCOMMANDHEAL_H_

#include "CombatCommandSystem.h"

namespace Descension
{
    class CombatCommandHeal: public CombatCommand
    {
    private:
        Simian::s32 healthIncrement_;
        Simian::f32 manaIncrement_;
    public:
        CombatCommandHeal();
        void Execute();
        static CombatCommandHeal* Create(Simian::s32 healthIncrement, Simian::f32 manaIncrement);
    };
}

#endif
