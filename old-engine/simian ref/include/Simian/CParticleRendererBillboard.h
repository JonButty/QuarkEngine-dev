/************************************************************************/
/*!
\file		CParticleRendererBillboard.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CPARTICLERENDERERBILLBOARD_H_
#define SIMIAN_CPARTICLERENDERERBILLBOARD_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

namespace Simian
{
    class ParticleRendererBillboard;
    class CParticleSystem;
    class MaterialCache;
    class Material;

    class SIMIAN_EXPORT CParticleRendererBillboard: public EntityComponent
    {
    private:
        ParticleRendererBillboard* renderer_;
        CParticleSystem* particleSystem_;
        std::string materialLocation_;
        Material* material_;
        bool local_;
        bool velocityOrient_;

        static Simian::MaterialCache* materialCache_;
        static Simian::GraphicsDevice* graphicsDevice_;
        static FactoryPlant<EntityComponent, CParticleRendererBillboard> factoryPlant_;
    public:
        ParticleRendererBillboard* Renderer() const;

        static Simian::GraphicsDevice* GraphicsDevice();
        static void GraphicsDevice(Simian::GraphicsDevice* val);

        static Simian::MaterialCache* MaterialCache();
        static void MaterialCache(Simian::MaterialCache* val);
    private:
        void initRenderer_();
        void update_(DelegateParameter& parameter);
    public:
        CParticleRendererBillboard();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();

        void Serialize( FileObjectSubNode& data );
        void Deserialize( const FileObjectSubNode& data );
    };
}

#endif
