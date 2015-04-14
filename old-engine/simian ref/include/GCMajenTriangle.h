/******************************************************************************/
/*!
\file		GCMajenTriangle.h
\author 	Bryan Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCMAJENTRIANGLE_H_
#define DESCENSION_GCMAJENTRIANGLE_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCAttributes;

    class GCMajenTriangle: public Simian::EntityComponent
    {
    private:
        Simian::CTransform* markers_[3];
        Simian::f32 castingTime_;
        Simian::f32 timer_;
        Simian::u32 damage_;
        Simian::Vector3 points_[3];
        GCAttributes* ownerAttribs_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCMajenTriangle> factoryPlant_;
    public:
        Simian::f32 CastingTime() const;
        void CastingTime(Simian::f32 val);
        void OwnerAttribs(GCAttributes* ownerAttribs);
        void Damage(Simian::u32 damage);

        const Simian::Vector3* Points() const;
        void Points(Simian::Vector3* val);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCMajenTriangle();

        void OnSharedLoad();
        void OnAwake();
    };
}

#endif
