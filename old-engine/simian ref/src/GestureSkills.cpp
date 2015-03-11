/*************************************************************************/
/*!
\file		GestureSkills.cpp
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GestureSkills.h"
#include "Simian/Utility.h"

namespace Descension {
    void GestureSkills::Skills(Simian::u32 val)
    {
        skills_ = val;
    }

    Simian::u32 GestureSkills::Skills(void) const
    {
        return skills_;
    }

    GestureSkills& GestureSkills::operator = (const GestureSkills& rhs)
    {
        skills_ = rhs.skills_;
        return *this;
    }
    //--------------------------------------------------------------------------
    GestureSkills::GestureSkills(Simian::u32 skills) : skills_(skills)
    {
        //--member init
    }
    //--------------------------------------------------------------------------
    bool GestureSkills::CheckSkill(Simian::u32 bitPos)
    {
        return SIsFlagSet(skills_, bitPos);
    }
}
