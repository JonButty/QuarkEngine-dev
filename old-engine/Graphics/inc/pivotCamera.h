#ifndef PIVOTCAMERA_H
#define PIVOTCAMERA_H

#include "vector3.h"
#include "spherical.h"
#include "camera.h"
#include "factory.h"
#include "configGraphicsDLL.h"
#include "inputDevice.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT PivotCamera
        :   public Camera
    {
    public:
        PivotCamera();
        PivotCamera(const PivotCamera& val);
        PivotCamera(const Math::Vec3F& origin,
                    const Math::SphericalF& polar);
    public:
        Math::SphericalF SphericalCoord() const;
        void SphericalCoord(const Math::SphericalF& val);
        Math::Vec3F Origin() const;
        void Origin(const Math::Vec3F& val);
    private:
        void load();
        void update(float dt);
    private:
        float zoomTimer_;
        float zoomOffset_;
        float rollTimer_;
        float unRollTimer_;
        float rollAngle_;
        float originOffsetTimer_;
        float unOffsetOriginTimer_;
        float originOffset_;
        float zoomSpeed_;
        float rotSpeed_;
        float tiltSpeed_;
        Math::Vec3F origin_;
        Math::SphericalF sphericalCoord_;
        static Util::FactoryPlant<Camera,PivotCamera> factoryPlant_;
    };
}

#endif