/******************************************************************************/
/*!
\file		AISkillWander.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AISKILLWANDER_H_
#define DESCENSION_AISKILLWANDER_H_

#include "AIEffectBase.h"
#include "AIEffectWander.h"

namespace Simian
{
    class CTransform;
    class CModel;
}

namespace Descension
{
    class GCAttributes;
    class GCPhysicsController;
    class AISkillWander : public AIEffectBase
    {
    private:
        AIEffectWander effect_;

        static Simian::FactoryPlant<AIEffectBase, AISkillWander> factoryPlant_;
    private:
        void onAwake_();
        void onInit_( GCAttributes* source );
        void onUnload_();
        void serialize_( Simian::FileObjectSubNode& node);
        void deserialize_( const Simian::FileObjectSubNode& node);
    public:
        AIEffectWander();
        ~AIEffectWander();
        bool Update( Simian::f32 dt);
    };
}

#endif
