/******************************************************************************/
/*!
\file		GCMousePlaneIntersection.cpp
\author 	Bryan Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "GCMousePlaneIntersection.h"
#include "ComponentTypes.h"
#include "GCEditor.h"

#include "Simian\EngineComponents.h"
#include "Simian\EnginePhases.h"
#include "Simian\Plane.h"
#include "Simian\Ray.h"
#include "Simian\CCamera.h"
#include "Simian\Camera.h"
#include "Simian\Mouse.h"
#include "Simian\CTransform.h"
#include "Simian\Math.h"

#include <iostream>

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCMousePlaneIntersection> GCMousePlaneIntersection::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_MOUSEPLANEINTERSECTION);

    GCMousePlaneIntersection::GCMousePlaneIntersection()
        : transform_(0),
          height_(0.0f),
          active_(true),
          intp_(1.0f)
    {
    }

    //--------------------------------------------------------------------------

    Simian::f32 GCMousePlaneIntersection::Height() const
    {
        return height_;
    }

    void GCMousePlaneIntersection::Height( Simian::f32 val )
    {
        height_ = val;
    }

    bool GCMousePlaneIntersection::Active() const
    {
        return active_;
    }

    void GCMousePlaneIntersection::Active( bool val )
    {
        if (val && !active_) //--Useful smooth out
        {
            InterpolateToPos(true);
            SetTargetPos(transform_->Position());
        }
        else if (!val && active_)
            InterpolateToPos(false);
        active_ = val;
    }

    Simian::CTransform* GCMousePlaneIntersection::Transform(void)
    {
        return transform_;
    }

    void GCMousePlaneIntersection::InterpolateToPos(bool flag)
    {
        if (flag)
            intp_ = 0.0f;
        //--Designed to be called only once
        interpol_ = flag;
    }

    void GCMousePlaneIntersection::SetTargetPos(const Simian::Vector3& targetPos)
    {
        targetPos_ = targetPos;
    }
    
    //--------------------------------------------------------------------------

    void GCMousePlaneIntersection::update_( Simian::DelegateParameter& param)
    {
#ifdef _DESCENDED
        if (GCEditor::Instance().EditorMode())
            return;
#endif
        if (interpol_) //--Interpolating mode to be used when inactive
        {
            if (active_ && intp_ >= 1.0f)
                interpol_ = false;
            //--Interpolate to position
            Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
            intp_ += Simian::Math::Clamp(dt*0.75f, 0.0f, 0.1f);
            intp_ = Simian::Math::Clamp(intp_, 0.0f, 1.0f);
            if (!active_)
                transform_->Position(transform_->Position()+
                (targetPos_-transform_->Position())*intp_);
        }
        // don't do anything unless active.
        if (!active_)
            return;

        Simian::CCamera* camera;
        Simian::CCameraBase::SceneCamera(camera);
        Simian::Vector3 intersection;

        if (Simian::Plane(Simian::Vector3(0, height_, 0), Simian::Vector3::UnitY).Intersect(
            Simian::Ray::FromProjection(camera->Camera().View().Inversed(),
            camera->Camera().Projection(), Simian::Mouse::GetMouseNormalizedPosition()), intersection))
        {
            if (transform_->ParentTransform())
                transform_->ParentTransform()->World().Inversed().Transform(intersection);
            transform_->Position(intersection);
        }

        if (interpol_) //--Get final pos and interpol
        {
            targetPos_ += (transform_->Position()-targetPos_)*intp_;
            transform_->Position(targetPos_);
        }
    }

    //--------------------------------------------------------------------------

    void GCMousePlaneIntersection::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCMousePlaneIntersection, &GCMousePlaneIntersection::update_>(this));
    }

    void GCMousePlaneIntersection::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
    }

    void GCMousePlaneIntersection::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Height", height_);
        data.AddData("Active", active_);
    }

    void GCMousePlaneIntersection::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("Height", height_, height_);
        data.Data("Active", active_, active_);
    }
}
