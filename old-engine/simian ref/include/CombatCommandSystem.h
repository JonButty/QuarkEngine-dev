/*************************************************************************/
/*!
\file		CombatCommandSystem.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_COMBATCOMMANDSYSTEM_H_
#define DESCENSION_COMBATCOMMANDSYSTEM_H_

#include "Simian/Singleton.h"
#include "Simian/StackAlloc.h"

#include "GCAttributes.h"

#include <vector>

namespace Descension
{
    // 100kb of stack.. should be more than enough
    static const Simian::u32 COMMAND_STACK_SIZE = 102400;

    class CombatCommand
    {
    protected:
        GCAttributes* source_;
        GCAttributes* target_;
    public:
        CombatCommand(): source_(0), target_(0) {}
        virtual ~CombatCommand() {}
        virtual void Execute() = 0;

        friend class CombatCommandSystem;
    };

    class CombatCommandSystem: public Simian::Singleton<CombatCommandSystem>
    {
    private:
        Simian::StackAlloc<COMMAND_STACK_SIZE> alloc_;
        std::vector<CombatCommand*> commands_;
    public:
        template <class T>
        void CreateCommand(T*& command)
        {
            command = alloc_.Alloc<T>();
        }
        void AddCommand(GCAttributes* source, GCAttributes* target, CombatCommand* command);
        void Process();
        void Clear();
    };
}

#endif
