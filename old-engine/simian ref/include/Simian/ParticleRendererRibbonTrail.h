/************************************************************************/
/*!
\file		ParticleRendererRibbonTrail.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_PARTICLERENDERERRIBBONTRAIL_H_
#define SIMIAN_PARTICLERENDERERRIBBONTRAIL_H_

#include "SimianPlatform.h"
#include "ParticleRenderer.h"
#include "VertexBuffer.h"
#include "Trail.h"
#include "Delegate.h"

namespace Simian
{
    class IndexBuffer;
    class GraphicsDevice;
    class Material;
    class ParticleSystem;

    class SIMIAN_EXPORT ParticleRendererRibbonTrail: public ParticleRenderer
    {
    private:
        VertexBuffer* vertexBuffer_;
        IndexBuffer* indexBuffer_;
        GraphicsDevice* device_;
        Simian::Material* material_;
        std::vector<PositionColorUVVertex> vertices_;
        std::vector<Trail> trails_;

        u32 segmentCount_;
        f32 interpolationInterval_;
        f32 segmentTwist_;
        f32 segmentTwistVelocity_;
        u32 trailCount_;
        bool local_;
    public:
        GraphicsDevice* Device() const;
        void Device(GraphicsDevice* val);

        Simian::Material* Material() const;
        void Material(Simian::Material* val);

        void SegmentCount(u32 count);
        void InterpolationInterval(Simian::f32 val);
        void SegmentTwist(f32 twist);
        void SegmentTwistVelocity(f32 velocity);

        bool Local() const;
        void Local(bool val);
    private:
        void render_(DelegateParameter&);
    public:
        ParticleRendererRibbonTrail(ParticleSystem* parent);

        void Load();
        void Unload();
        void Update( f32 dt );
        void Render();
        void Render( RenderQueue& renderQueue, u8 layer, const Simian::Matrix& transform );

        void OnSpawn( Particle& particle );
    };
}

#endif
