/******************************************************************************/
/*!
\file		AITimedEffectFreeze.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AITIMEDEFFECTFREEZE_H_
#define DESCENSION_AITIMEDEFFECTFREEZE_H_

#include "AITimedEffectBase.h"

namespace Descension
{
    class GCAttributes;

    class AITimedEffectFreeze : public AITimedEffectBase
    {
    private:
        GCAttributes* victimAttrib_;
        static Simian::FactoryPlant<AIEffectBase, AITimedEffectFreeze> factoryPlant_;
    public:
    protected:
        void onAwake_();
        void onInit_( GCAttributes* source);
        void onUnload_();
        void serialize_( Simian::FileObjectSubNode& node );
        void deserialize_( const Simian::FileObjectSubNode& node );
        bool update_( Simian::f32 dt );
    public:
        AITimedEffectFreeze();
        ~AITimedEffectFreeze();
    };
}

#endif
