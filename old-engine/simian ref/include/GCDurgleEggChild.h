/******************************************************************************/
/*!
\file		GCDurgleEggChild.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/12/07
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCDURGLEEGGCHILD_H_
#define DESCENSION_GCDURGLEEGGCHILD_H_

#include "Simian/EntityComponent.h"
#include <string>

namespace Simian
{
    class CModel;
    class CTransform;
}

namespace Descension
{
    class GCDurgleEggChild : public Simian::EntityComponent
    {
    private:
        Simian::CTransform* trans_;
        Simian::CModel* model_;
        bool start_;
        Simian::f32 maxTime_;
        Simian::f32 timer_;
        Simian::Vector3 scaleIncrease_;
        Simian::Vector3 finalScale_;
        static Simian::FactoryPlant<Simian::EntityComponent, GCDurgleEggChild> factoryPlant_;
    public:
        void Start();
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCDurgleEggChild();
        ~GCDurgleEggChild();
        void OnSharedLoad();
        void OnInit();
        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}
#endif
