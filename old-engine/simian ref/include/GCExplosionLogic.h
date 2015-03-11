/******************************************************************************/
/*!
\file		GCExplosionLogic.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCEXPLOSIONLOGIC_H_
#define DESCENSION_GCEXPLOSIONLOGIC_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCAttributes;

    class GCExplosionLogic : public Simian::EntityComponent
    {
    private:
        enum ExplosionState
        {
            ES_EXPLODE,
            ES_DESTROY,
            ES_TOTAL
        };
    private:
        Simian::CTransform* targetTrans_;
        Simian::f32 timer_, maxTime_;

        // factory plant
        static Simian::FactoryPlant<Simian::EntityComponent, GCExplosionLogic> factoryPlant_;
    public:
        void MaxTime(Simian::f32 val);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCExplosionLogic();
        void OnSharedLoad();
        void OnAwake();
        void OnInit();
        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);        
    };
}
#endif
