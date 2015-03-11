/************************************************************************/
/*!
\file		CParticleRendererRibbonTrail.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CPARTICLERENDERERRIBBONTRAIL_H_
#define SIMIAN_CPARTICLERENDERERRIBBONTRAIL_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

#include <string>

namespace Simian
{
    class ParticleRendererRibbonTrail;
    class CParticleSystem;
    class Material;

    class SIMIAN_EXPORT CParticleRendererRibbonTrail: public EntityComponent
    {
    private:
        ParticleRendererRibbonTrail* renderer_;
        CParticleSystem* particleSystem_;
        std::string materialLocation_;
        Material* material_;

        static MaterialCache* materialCache_;
        static GraphicsDevice* graphicsDevice_;
        static FactoryPlant<EntityComponent, CParticleRendererRibbonTrail> factoryPlant_;

        u32 segmentCount_;
        f32 interpolationInterval_;
        f32 segmentTwist_;
        f32 segmentTwistVelocity_;
        bool local_;
    public:
        ParticleRendererRibbonTrail* Renderer() const;

        static Simian::MaterialCache* MaterialCache();
        static void MaterialCache(Simian::MaterialCache* val);

        static Simian::GraphicsDevice* GraphicsDevice();
        static void GraphicsDevice(Simian::GraphicsDevice* val);
    private:
        void initRenderer_();
    public:
        CParticleRendererRibbonTrail();

        void OnSharedLoad();
        void OnAwake();

        void Serialize( FileObjectSubNode& );
        void Deserialize( const FileObjectSubNode& );
    };
}

#endif
