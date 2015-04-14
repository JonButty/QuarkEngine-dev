/*************************************************************************/
/*!
\file		AITimedEffectBase.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "AITimedEffectBase.h"
#include "Simian\FileObjectSubNode.h"

namespace Descension
{
    AITimedEffectBase::AITimedEffectBase(AIEffectType type)
        :   AIEffectBase(type),
            timer_(0)
    {
    }

    AITimedEffectBase::~AITimedEffectBase()
    {
    }

    //-Public-------------------------------------------------------------------

    Simian::f32 AITimedEffectBase::Timer() const
    {
        return timer_;
    }

    void AITimedEffectBase::Timer( Simian::f32 time )
    {
        timer_ = time;
    }

    bool AITimedEffectBase::Update( Simian::f32 dt )
    {
        timer_ -= dt;
        if(timer_ > 0.0f)
            return update_(dt);
        return false;
    }

    void AITimedEffectBase::Serialize( Simian::FileObjectSubNode& node )
    {
        //node.AddData("Time",timer_);
        serialize_(node);
    }

    void AITimedEffectBase::Deserialize( const Simian::FileObjectSubNode& node )
    {
        node.Data("Time",timer_,timer_);
        deserialize_(node);
    }
}
