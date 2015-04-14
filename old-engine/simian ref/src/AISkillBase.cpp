/*************************************************************************/
/*!
\file		AISkillBase.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "AISkillBase.h"
#include "Simian\FileDataSubNode.h"

namespace Descension
{
    AISkillBase::AISkillBase( AIEffectType type )
        :   AIEffectBase(type)
    {
        Skill(true);
    }

    AISkillBase::~AISkillBase()
    {
    }

    //--------------------------------------------------------------------------

    Simian::u32 AISkillBase::Chance() const
    {
        return chance_;
    }

    void AISkillBase::Chance( Simian::u32 val )
    {
        chance_ = val;
    }

    void AISkillBase::Serialize( Simian::FileObjectSubNode& node )
    {
        node.AddData("Chance",chance_);
        node.AddData("Enable",enable_);
        serialize_(node);
    }

    void AISkillBase::Deserialize( const Simian::FileObjectSubNode& node )
    {
        const Simian::FileDataSubNode* data = node.Data("Chance");
        chance_ = data? data->AsU32() : chance_;

        data = node.Data("Enable");
        enable_ = data? data->AsBool() : enable_;

        deserialize_(node);
    }
}
