/******************************************************************************/
/*!
\file		GCMousePlaneIntersection.h
\author 	Bryan Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_MOUSEPLANEINTERSECTION_H_
#define DESCENSION_MOUSEPLANEINTERSECTION_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCMousePlaneIntersection: public Simian::EntityComponent
    {
    private:
        Simian::CTransform* transform_;
        Simian::f32 height_;
        bool active_, interpol_;

        Simian::f32 intp_;
        Simian::Vector3 targetPos_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCMousePlaneIntersection> factoryPlant_;
    public:
        Simian::f32 Height() const;
        void Height(Simian::f32 val);

        bool Active() const;
        void Active(bool val);

        Simian::CTransform* Transform(void);
        void InterpolateToPos(bool flag);
        void SetTargetPos(const Simian::Vector3& targetPos);
    private:
        void update_(Simian::DelegateParameter&);
    public:
        GCMousePlaneIntersection();

        void OnSharedLoad();
        void OnAwake();

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
