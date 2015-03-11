/******************************************************************************/
/*!
\file		AIEffectCreep.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_AIEFFECTWALK_H_
#define DESCENSION_AIEFFECTWALK_H_

#include "Simian/SimianTypes.h"
#include "Simian/Delegate.h"
#include "Simian/Vector3.h"

#include "AIEffectBase.h"

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCTileMap;

    class AIEffectCreep : public AIEffectBase
    {
    private:
        GCTileMap* map_;
        Simian::CTransform* targetTransform_;
        Simian::CTransform* sourceTransform_;
        Simian::f32 speedRatio_;
        Simian::f32 creepRadiusSqr_;
        Simian::f32 stopRadiusSqr_;
        Simian::f32 creepStartDist_;
        Simian::f32 creepEndDist_;
        Simian::f32 originalMoveSpeed_;

        static Simian::FactoryPlant<AIEffectBase, AIEffectCreep> factoryPlant_;
    private:
        void onAwake_();
        void onInit_( GCAttributes* source );
        void onUnload_();
        void serialize_( Simian::FileObjectSubNode& node );
        void deserialize_( const Simian::FileObjectSubNode& node );
    public:
        AIEffectCreep();
        bool Update( Simian::f32 dt );
    };
}

#endif
