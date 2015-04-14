/*************************************************************************/
/*!
\file		TimedEffect.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "TimedEffect.h"

#include "AIEffectBase.h"

namespace Descension
{
    TimedEffect::TimedEffect(AIEffectBase* effect, Simian::f32 timer)
        :   effect_(effect),
            timer_(timer)
    {
    }

    //-Public-------------------------------------------------------------------

    AIEffectBase* TimedEffect::Effect() const
    {
        return effect_;
    }

    Simian::f32 TimedEffect::Timer() const
    {
        return timer_;
    }

    //-Public-------------------------------------------------------------------

    void TimedEffect::Load(Simian::Entity* entity)
    {
        entity;
        //effect_->OnLoad();
    }

    void TimedEffect::Unload()
    {
        effect_->OnUnload();
    }

    bool TimedEffect::Update( Simian::f32 dt )
    {
        if(timer_ > 0)
        {
            timer_ -= dt;
            return effect_->Update(dt);
        }
        // Send Remove Effect Command
        return false;
    }
}
