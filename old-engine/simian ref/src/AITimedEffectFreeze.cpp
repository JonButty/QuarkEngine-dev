/******************************************************************************/
/*!
\file		AITimedEffectFreeze.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "AITimedEffectFreeze.h"

#include "GCAttributes.h"

namespace Descension
{
    Simian::FactoryPlant<AIEffectBase, AITimedEffectFreeze> AITimedEffectFreeze::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AITimedEffectFreeze>(EFF_T_FREEZE));

    AITimedEffectFreeze::AITimedEffectFreeze()
        :   AITimedEffectBase(EFF_T_FREEZE),
            victimAttrib_(0)
    {
    }

    AITimedEffectFreeze::~AITimedEffectFreeze()
    {

    }

    //-Private------------------------------------------------------------------

    void AITimedEffectFreeze::onAwake_()
    {

    }

    void AITimedEffectFreeze::onInit_( GCAttributes* source)
    {
        if(source)
        {

        }
        else
        {
            //GCPlayerLogic::Instance()->pl
        }
        victimAttrib_ = Target()->Victim();
    }

    void AITimedEffectFreeze::onUnload_()
    {

    }

    void AITimedEffectFreeze::serialize_( Simian::FileObjectSubNode& node )
    {
        node;
    }

    void AITimedEffectFreeze::deserialize_( const Simian::FileObjectSubNode& node )
    {
        node;
    }

    bool AITimedEffectFreeze::update_( Simian::f32 dt )
    {
        dt;
        return false;
    }
}
