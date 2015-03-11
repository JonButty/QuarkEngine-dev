/************************************************************************/
/*!
\file		ParticleRendererBillboard.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_PARTICLERENDERERBILLBOARD_H_
#define SIMIAN_PARTICLERENDERERBILLBOARD_H_

#include "SimianPlatform.h"
#include "ParticleRenderer.h"
#include "VertexBuffer.h"

namespace Simian
{
    class VertexBuffer;
    class IndexBuffer;
    class GraphicsDevice;
    class Camera;
    class Material;
    struct DelegateParameter;

    class SIMIAN_EXPORT ParticleRendererBillboard: public ParticleRenderer
    {
    private:
        VertexBuffer* vertexBuffer_;
        IndexBuffer* indexBuffer_;
        GraphicsDevice* device_;
        Simian::Camera* camera_;
        Simian::Material* material_;
        std::vector<PositionColorUVVertex> vertices_;
        bool local_;
        bool velocityOrient_;
    public:
        GraphicsDevice* Device() const;
        void Device(GraphicsDevice* val);

        Simian::Camera* Camera() const;
        void Camera(Simian::Camera* val);

        Simian::Material* Material() const;
        void Material(Simian::Material* val);

        bool Local() const;
        void Local(bool val);

        bool VelocityOrient() const;
        void VelocityOrient(bool val);
    private:
        void render_(DelegateParameter& param);
    public:
        ParticleRendererBillboard(ParticleSystem* parent);
        
        void Load();
        void Unload();
        void Update( f32 dt );
        void Render();
        void Render( RenderQueue& renderQueue, u8 layer, const Simian::Matrix& transform);
    };
}

#endif
