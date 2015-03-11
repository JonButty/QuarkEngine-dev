#include "cameraDX.h"
#include "configGFX.h"
#include "gfxLogger.h"

#include <complex>

namespace GFX
{
    static const D3DXVECTOR3 DEFAULT_LOOKAT(0,0,1);
    static const D3DXVECTOR3 DEFAULT_UP(0,1,0);

    Camera::Camera(unsigned int type,
                   bool enable,
                   Math::RadianF xAxis,
                   Math::RadianF yAxis,
                   Math::RadianF zAxis,
                   Math::Vec3F pos)
                   :    type_(type),
                        enable_(enable),
                        pos_(pos),
                        xAxisRot_(xAxis),
                        yAxisRot_(yAxis),
                        zAxisRot_(zAxis)
    {
    }

    Camera::Camera( const Camera& )
    {

    }

    Camera::~Camera()
    {
    }

    void Camera::Load()
    {
        load();
    }

    void Camera::Update(float dt)
    {
        if(enable_)
            update(dt);
        
        D3DXVECTOR3 posDX(pos_.X(), pos_.Y(),pos_.Z());

        // Create the rotation matrix from the yaw, pitch, and roll values.
        D3DXMATRIX rotationMatrix;
        D3DXMatrixRotationYawPitchRoll(&rotationMatrix,
                                       -yAxisRot_.AsRadians(),
                                        xAxisRot_.AsRadians(),
                                        zAxisRot_.AsRadians());
        
        // Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
        D3DXVECTOR3 lookDX, upDX;
        D3DXVec3TransformCoord(&lookDX, &DEFAULT_LOOKAT, &rotationMatrix);
        D3DXVec3TransformCoord(&upDX, &DEFAULT_UP, &rotationMatrix);

        // Translate the rotated camera position to the location of the viewer.
        lookDX = posDX + lookDX;

        // Finally create the view matrix from the three updated vectors.
        D3DXMatrixLookAtLH(&worldToView_, &posDX, &lookDX, &upDX);
    }

    void Camera::Unload()
    {
        unload();
    }

    //--------------------------------------------------------------------------

    Math::Vec3F Camera::Pos() const
    {
        return pos_;
    }

    void Camera::Pos( const Math::Vec3F& val )
    {
        pos_ = val;
    }

    void Camera::Pos( float x,
                      float y,
                      float z ) 
    {
        pos_.X(x);
        pos_.Y(y);
        pos_.Z(z);
    }

    void Camera::ZAxisRotation( Math::RadianF val )
    {
        zAxisRot_ = val;
    }

    Math::RadianF Camera::ZAxisRotation() const
    {
        return zAxisRot_;
    }

    void Camera::YAxisRotation( Math::RadianF val )
    {
        yAxisRot_ = val;
    }

    Math::RadianF Camera::YAxisRotation() const
    {
        return yAxisRot_;
    }

    void Camera::XAxisRotation( Math::RadianF val )
    {
        xAxisRot_ = val;
    }

    Math::RadianF Camera::XAxisRotation() const
    {
        return xAxisRot_;
    }

    D3DXMATRIX Camera::WorldToView() const
    {
        return worldToView_;
    }

    unsigned int Camera::Type() const
    {
        return type_;
    }

    bool Camera::Enable() const
    {
        return enable_;
    }

    void Camera::Enable( bool val )
    {
        enable_ = val;
    }

    //--------------------------------------------------------------------------

    void Camera::load()
    {
    }

    void Camera::update(float)
    {
    }

    void Camera::unload()
    {
    }
}