/*************************************************************************/
/*!
\file		GestureSkills.h
\author 	Ngai Wen Sheng, Jason - jason.ngai - 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/08/30
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/

#include "Simian/SimianTypes.h"

namespace Descension {
    class GestureSkills {
        
    private: //--member data
        Simian::u32 skills_; //--bit flags used for storing skills

    public: //--settors & accessors
        void Skills(Simian::u32 val);
        Simian::u32 Skills(void) const;

        GestureSkills& operator = (const GestureSkills& rhs);
        operator int() {return skills_;}

    public: //--ctors
        GestureSkills(Simian::u32 skills = 0);

    public: //--public methods
        bool CheckSkill(Simian::u32 bitPos);

    };
}
