/************************************************************************/
/*!
\file		Camera.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Camera.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/Utility.h"
#include "Simian/RenderTexture.h"
#include "Simian/Material.h"
#include "Simian/AABB.h"
#include "Simian/CompositorChain.h"

namespace Simian
{
    Camera::Camera( GraphicsDevice* device )
        : device_(device),
          renderTexture_(0),
          compositorChain_(0),
          clearColor_(0.0f, 0.0f, 0.0f, 1.0f),
          layers_(0),
          composite_(true),
          dirty_(true)
    {
    }

    //--------------------------------------------------------------------------

    GraphicsDevice& Camera::Device() const
    {
        return *device_;
    }

    const std::string& Camera::RenderTextureName() const
    {
        return renderTextureName_;
    }

    void Camera::RenderTextureName( const std::string& val )
    {
        renderTextureName_ = val;
        if (renderTextureName_ != "")
            Material::RenderTextures()[renderTextureName_] = renderTexture_;
    }

    Simian::RenderTexture* Camera::RenderTexture() const
    {
        return renderTexture_;
    }

    void Camera::RenderTexture( Simian::RenderTexture* val )
    {
        renderTexture_ = val;
    }

    Simian::Vector3 Camera::Direction() const
    {
        return Simian::Vector3(
            view_[2][0], view_[2][1], view_[2][2]);
    }

    const Simian::Matrix& Camera::View()
    {
        return view_;
    }

    void Camera::View( const Simian::Matrix& val )
    {
        view_ = val;
        dirty_ = true;
    }

    const Simian::Matrix& Camera::Projection()
    {
        return projection_;
    }

    void Camera::Projection( const Simian::Matrix& val )
    {
        projection_ = val;
        dirty_ = true;
    }

    const Simian::Color& Camera::ClearColor() const
    {
        return clearColor_;
    }

    void Camera::ClearColor( const Simian::Color& val )
    {
        clearColor_ = val;
    }

    Simian::u32 Camera::Layers() const
    {
        return layers_;
    }

    void Camera::Layers( Simian::u32 val )
    {
        layers_ = val;
    }

    bool Camera::Composite() const
    {
        return composite_;
    }

    void Camera::Composite( bool val )
    {
        composite_ = val;
    }

    Simian::CompositorChain* Camera::CompositorChain() const
    {
        return compositorChain_;
    }

    void Camera::CompositorChain( Simian::CompositorChain* val )
    {
        compositorChain_ = val;
        compositorChain_->Input(renderTexture_);
    }

    //--------------------------------------------------------------------------

    void Camera::updateFrustum_()
    {
        // no need to update frustum if not dirty
        if (!dirty_)
            return;
        dirty_ = false;
         
        // compute view projection matrix
        Matrix fmat = projection_ * view_;
        Simian::Vector4& r0 = *reinterpret_cast<Simian::Vector4*>(fmat[0]);
        Simian::Vector4& r1 = *reinterpret_cast<Simian::Vector4*>(fmat[1]);
        Simian::Vector4& r2 = *reinterpret_cast<Simian::Vector4*>(fmat[2]);
        Simian::Vector4& r3 = *reinterpret_cast<Simian::Vector4*>(fmat[3]);

        frustum_[0] = r3 + r0;
        frustum_[1] = r3 - r0;
        frustum_[2] = r3 + r1;
        frustum_[3] = r3 - r1;
        frustum_[4] = r2;
        frustum_[5] = r3 - r2;
    }

    //--------------------------------------------------------------------------

    void Camera::Set()
    {
        if (renderTexture_)
            renderTexture_->Use();

        // begin scene
        device_->Begin();

        // clear
        device_->Clear(clearColor_);
    }

    void Camera::Unset()
    {
        // end scene
        device_->End();

        if (renderTexture_)
            renderTexture_->Unuse();
    }

    bool Camera::TestAABB( const AABB& aabb )
    {
        updateFrustum_();

        Simian::Vector4 pts[] = 
        {
            Simian::Vector4(aabb.Min(), 1),
            Simian::Vector4(aabb.Min().X(), aabb.Min().Y(), aabb.Max().Z(), 1),
            Simian::Vector4(aabb.Min().X(), aabb.Max().Y(), aabb.Min().Z(), 1),
            Simian::Vector4(aabb.Min().X(), aabb.Max().Y(), aabb.Max().Z(), 1),
            Simian::Vector4(aabb.Max().X(), aabb.Min().Y(), aabb.Min().Z(), 1),
            Simian::Vector4(aabb.Max().X(), aabb.Min().Y(), aabb.Max().Z(), 1),
            Simian::Vector4(aabb.Max().X(), aabb.Max().Y(), aabb.Min().Z(), 1),
            Simian::Vector4(aabb.Max(), 1),
        };

        for (u32 i = 0; i < 6; ++i)
        {
            u32 inPoints = 0;
            for (u32 j = 0; j < 8; ++j)
            {
                // the point is in the half plane
                if (frustum_[i].Dot(pts[j]) > 0)
                {
                    ++inPoints;
                    // we can stop testing
                    break;
                }
            }

            // no point is in the plane
            if (!inPoints)
                return false;
        }

        return true;
    }

    void Camera::AddLayer( u8 layer )
    {
        SFlagSet(layers_, layer);
    }

    void Camera::RemoveLayer( u8 layer )
    {
        SFlagUnset(layers_, layer);
    }
}
