/******************************************************************************/
/*!
\file		GCCrystalSwap.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/12/07
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCCRYSTALSWAP_H_
#define DESCENSION_GCCRYSTALSWAP_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
    class CModel;
}

namespace Descension
{
    class GCCrystalSwap : public Simian::EntityComponent
    {
    public:
        enum CrystalType
        {
            CT_FIRE,
            CT_LIGHTNING,
            CT_TOTAL
        };
    private:
        Simian::CTransform* targetTrans_;
        Simian::CTransform* trans_;
        Simian::CModel* crystals[CT_TOTAL];
        std::string targetName_;
        static Simian::FactoryPlant<Simian::EntityComponent, GCCrystalSwap> factoryPlant_;
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCCrystalSwap();
        ~GCCrystalSwap();
        void OnSharedLoad();
        void OnInit();
        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
        void SetActiveCrystal(unsigned int type);
    };
}
#endif
