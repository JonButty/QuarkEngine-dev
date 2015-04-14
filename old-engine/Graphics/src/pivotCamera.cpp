#include "pivotCamera.h"
#include "cameraTypes.h"
#include "cameraFactory.h"
#include "inputDeviceManager.h"
#include "keyboard.h"
#include "gfxLogger.h"
#include "interpolate.h"
#include "mathDef.h"

namespace GFX
{
    Util::FactoryPlant<Camera,PivotCamera> PivotCamera::factoryPlant_(CameraFactory::InstancePtr(),C_PIVOT);

    PivotCamera::PivotCamera()
        :   Camera(C_PIVOT),
            zoomSpeed_(0.75f),
            rotSpeed_(60.f),
            tiltSpeed_(60.f),
            zoomOffset_(0),
            origin_(Math::Vec3F::Zero)
    {
    }

    PivotCamera::PivotCamera( const PivotCamera& val )
        :   Camera(val),
            zoomSpeed_(val.zoomSpeed_),
            rotSpeed_(val.rotSpeed_),
            tiltSpeed_(val.tiltSpeed_),
            origin_(val.origin_),
            sphericalCoord_(val.sphericalCoord_)
    {
    }

    PivotCamera::PivotCamera( const Math::Vec3F& origin,
                              const Math::SphericalF& spherical )
                              : Camera(C_PIVOT),
                                origin_(origin),
                                sphericalCoord_(spherical)
    {
    }

    //--------------------------------------------------------------------------

    Math::SphericalF PivotCamera::SphericalCoord() const
    {
        return sphericalCoord_;
    }

    void PivotCamera::SphericalCoord( const Math::SphericalF& val )
    {
        sphericalCoord_ = val;
    }

    Math::Vec3F PivotCamera::Origin() const
    {
        return origin_;
    }

    void PivotCamera::Origin( const Math::Vec3F& val )
    {
        origin_ = val;
    }

    //--------------------------------------------------------------------------

    void PivotCamera::load()
    {
        sphericalCoord_.Radius(1.f);
        rollTimer_ = 0.f;
    }

    void PivotCamera::update(float dt)
    {
        Util::InputDeviceManager* inputMgr = Util::InputDeviceManager::InstancePtr();
        float rollTime = 0.15f;
        float rollAngle = 1.75f;
        float unRollTime = 0.15f;
        float originOffsetTime = 0.25f;
        float originOffset = 0.03f;
        float unOffsetOriginTime = 0.25f;
        float zoomTime = 0.25f;
        float zoomOffset = 1.5f;

        /*
        if(inputMgr->DevicePressed(keyboard_,Util::K_S))
        {
            sphericalCoord_.Radius(sphericalCoord_.Radius() + zoomSpeed_ * dt);
            zoomOffset_ = zoomOffset;
            zoomTimer_ = 0.f;
        }
        else */
        if(zoomOffset_ > 0)
        {
            if(zoomTimer_ < zoomTime)
                zoomTimer_ += dt;
            zoomOffset_ = Math::Interpolate(zoomOffset,0.f,Math::Clamp(zoomTimer_/zoomTime,0.f,1.f),&Math::Interpolation<float>::EaseCubicOut);
        }
        
        /*if(inputMgr->DevicePressed(keyboard_,Util::K_W))
        {
            sphericalCoord_.Radius(sphericalCoord_.Radius() - zoomSpeed_ * dt);
            zoomOffset_ = -zoomOffset;
            zoomTimer_ = 0.f;
        }
        else */
        if(zoomOffset_ < 0)
        {
            if(zoomTimer_ < zoomTime)
                zoomTimer_ += dt;
            zoomOffset_ = Math::Interpolate(-zoomOffset,0.f,Math::Clamp(zoomTimer_/zoomTime,0.f,1.f),&Math::Interpolation<float>::EaseCubicOut);
        }

        sphericalCoord_.Radius(sphericalCoord_.Radius() + zoomOffset_  * sphericalCoord_.Radius() * dt);
        
        /*if(inputMgr->DevicePressed(keyboard_,Util::K_LEFT))
        {
            sphericalCoord_.ThetaHoriz(sphericalCoord_.ThetaHoriz() - Math::DegreeF(rotSpeed_).AsRadians() * dt);
            unRollTimer_ = 0.f;

            if(rollTimer_ < rollTime)
                rollTimer_ += dt;
            rollAngle_ = Math::Interpolate(0.f,-rollAngle,Math::Clamp(rollTimer_/rollTime,0.f,1.f),&Math::Interpolation<float>::EaseSquareIn);
        }
        else*/
        if(rollAngle_ < 0)
        {
            rollTimer_ = 0;
            if(unRollTimer_ < unRollTime)
                unRollTimer_ += dt;
            rollAngle_ = Math::Interpolate(-rollAngle,0.f,Math::Clamp(unRollTimer_/unRollTime,0.f,1.f),&Math::Interpolation<float>::EaseCubicOut);
        }
        
        /*if(inputMgr->DevicePressed(keyboard_,Util::K_RIGHT))
        {
            sphericalCoord_.ThetaHoriz(sphericalCoord_.ThetaHoriz() + Math::DegreeF(rotSpeed_).AsRadians() * dt);
            unRollTimer_ = 0.f;

            if(rollTimer_ < rollTime)
                rollTimer_ += dt;
            rollAngle_ = Math::Interpolate(0.f,rollAngle,Math::Clamp(rollTimer_/rollTime,0.f,1.f),&Math::Interpolation<float>::EaseSquareIn);
        }
        else*/
        if(rollAngle_ > 0)
        {
            rollTimer_ = 0;
            if(unRollTimer_ < unRollTime)
                unRollTimer_ += dt;
            rollAngle_ = Math::Interpolate(rollAngle,0.f,Math::Clamp(unRollTimer_/unRollTime,0.f,1.f),&Math::Interpolation<float>::EaseCubicOut);
        }

        /*if(inputMgr->DevicePressed(keyboard_,Util::K_UP))
        {
            sphericalCoord_.ThetaVert(sphericalCoord_.ThetaVert() + Math::DegreeF(tiltSpeed_).AsRadians() * dt);
            unOffsetOriginTimer_ = 0.f;

            if(originOffsetTimer_ < originOffsetTime)
                originOffsetTimer_ += dt;
            originOffset_ = Math::Interpolate(0.f,originOffset,Math::Clamp(originOffsetTimer_/originOffsetTime,0.f,1.f),&Math::Interpolation<float>::EaseSquareIn);
        }
        else*/
        if(originOffset_ > 0)
        {
            originOffsetTimer_ = 0;
            if(unOffsetOriginTimer_ < unOffsetOriginTime)
                unOffsetOriginTimer_ += dt;
            originOffset_ = Math::Interpolate(originOffset,0.f,Math::Clamp(unOffsetOriginTimer_/unOffsetOriginTime,0.f,1.f),&Math::Interpolation<float>::EaseCubicOut);
        }

        /*else if(inputMgr->DevicePressed(keyboard_,Util::K_DOWN))
        {
            sphericalCoord_.ThetaVert(sphericalCoord_.ThetaVert() - Math::DegreeF(tiltSpeed_).AsRadians() * dt);
            unOffsetOriginTimer_ = 0.f;

            if(originOffsetTimer_ < originOffsetTime)
                originOffsetTimer_ += dt;
            originOffset_ = Math::Interpolate(0.f,-originOffset,Math::Clamp(originOffsetTimer_/originOffsetTime,0.f,1.f),&Math::Interpolation<float>::EaseSquareIn);
        }
        else*/
        if(originOffset_ < 0)
        {
            originOffsetTimer_ = 0;
            if(unOffsetOriginTimer_ < unOffsetOriginTime)
                unOffsetOriginTimer_ += dt;
            originOffset_ = Math::Interpolate(-originOffset,0.f,Math::Clamp(unOffsetOriginTimer_/unOffsetOriginTime,0.f,1.f),&Math::Interpolation<float>::EaseCubicOut);
        }

        Math::Vec3F offset(sphericalCoord_.AsCartesian());
        
        // Rotate offset -90 (assuming default lookat is 0,0,1)
        float offsetAngle = Math::DegreeF(-90).AsRadians();
        float x = offset.X() * std::cos(offsetAngle) - offset.Z() * std::sin(offsetAngle);
        float z = offset.X() * std::sin(offsetAngle) + offset.Z() * std::cos(offsetAngle);
        offset.X(x);
        offset.Z(z);
        Pos(origin_ + Math::Vec3F(0,originOffset_*sphericalCoord_.Radius(),0) + offset);

        YAxisRotation(sphericalCoord_.ThetaHoriz().AsRadians());
        XAxisRotation(sphericalCoord_.ThetaVert().AsRadians());
        ZAxisRotation(Math::DegreeF(rollAngle_).AsRadians());
    }
}