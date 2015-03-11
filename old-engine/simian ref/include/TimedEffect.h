/******************************************************************************/
/*!
\file		TimedEffect.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_TIMEDEFFECT_H_
#define DESCENSION_TIMEDEFFECT_H_

#include "Simian/SimianTypes.h"

namespace Simian
{
    class Entity;
}

namespace Descension
{
    class AIEffectBase;

    class TimedEffect
    {
    private:
        AIEffectBase* effect_;
        Simian::f32 timer_;
    public:
        AIEffectBase* Effect() const;
        Simian::f32 Timer() const;
    public:
        TimedEffect(AIEffectBase* effect, Simian::f32 timer);

        void Load(Simian::Entity* target);
        void Unload();

        bool Update(Simian::f32 dt);
    };
}

#endif
