/******************************************************************************/
/*!
\file		AIEffectCreateEntity.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/11/01
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AIEFFECTCREATEENTITY_H_
#define DESCENSION_AIEFFECTCREATEENTITY_H_

#include "AIEffectBase.h"

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class AIEffectCreateEntity : public AIEffectBase
    {
    private:
        bool created_;
        Simian::CTransform* ownerTrans_;
        Simian::u32 entityType_;
        Simian::u32 stateTrigger_;
        static Simian::FactoryPlant<AIEffectBase, AIEffectCreateEntity> factoryPlant_;
    private:
        void onAwake_();
        void onInit_( GCAttributes* source );
        void onUnload_();
        void serialize_( Simian::FileObjectSubNode& node);
        void deserialize_( const Simian::FileObjectSubNode& node);
    public:
        AIEffectCreateEntity();
        ~AIEffectCreateEntity();
        bool Update( Simian::f32 dt);
    };
}

#endif
