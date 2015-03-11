/************************************************************************/
/*!
\file		Camera.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CAMERA_H_
#define SIMIAN_CAMERA_H_

#include "SimianPlatform.h"
#include "Vector3.h"
#include "Matrix.h"
#include "Angle.h"
#include "Color.h"

#include <string>

namespace Simian
{
    class GraphicsDevice;
    class RenderTexture;
    class AABB;
    class CompositorChain;

    // TODO: specialize for orthographic/perspective?
    class SIMIAN_EXPORT Camera
    {
    private:
        GraphicsDevice* device_;

        std::string renderTextureName_;
        RenderTexture* renderTexture_;
        CompositorChain* compositorChain_;

        Simian::Matrix view_;
        Simian::Matrix projection_;
        Simian::Vector4 frustum_[6];

        Simian::Color clearColor_;

        u32 layers_;

        bool composite_;
        bool dirty_;
    public:
        GraphicsDevice& Device() const;

        const std::string& RenderTextureName() const;
        void RenderTextureName(const std::string& val);

        Simian::RenderTexture* RenderTexture() const;
        void RenderTexture(Simian::RenderTexture* val);

        Simian::Vector3 Direction() const;

        const Simian::Matrix& View();
        void View(const Simian::Matrix& val);

        const Simian::Matrix& Projection();
        void Projection(const Simian::Matrix& val);

        const Simian::Color& ClearColor() const;
        void ClearColor(const Simian::Color& val);

        Simian::u32 Layers() const;
        void Layers(Simian::u32 val);

        bool Composite() const;
        void Composite(bool val);

        Simian::CompositorChain* CompositorChain() const;
        void CompositorChain(Simian::CompositorChain* val);
    private:
        void updateFrustum_();
    public:
        Camera(GraphicsDevice* device);

        void Set();
        void Unset();

        bool TestAABB(const AABB& aabb);

        void AddLayer(u8 layer);
        void RemoveLayer(u8 layer);
    };
}

#endif
