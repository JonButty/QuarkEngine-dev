/************************************************************************/
/*!
\file		CParticleSystem.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CPARTICLESYSTEM_H_
#define SIMIAN_CPARTICLESYSTEM_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"
#include "Vector3.h"
#include "Color.h"

namespace Simian
{
    class ParticleSystem;
    struct ParticleSystemFrame;
    class CTransform;
    class RenderQueue;

    class SIMIAN_EXPORT CParticleSystem: public EntityComponent
    {
    private:
        ExplicitType<Simian::ParticleSystem*> particleSystem_;
        CTransform* transform_;
        u32 totalParticles_;
        u8 layer_;

        static Simian::RenderQueue* renderQueue_;
        static FactoryPlant<EntityComponent, CParticleSystem> factoryPlant_;
    public:
        Simian::ParticleSystem* ParticleSystem() const;

        Simian::u8 Layer() const;

        static Simian::RenderQueue* RenderQueue();
        static void RenderQueue(Simian::RenderQueue* val);
    public:
        CParticleSystem& operator=(const CParticleSystem& particles);
    private:
        void parseColor_(Color& color, const FileObjectSubNode* colorNode);
        void parseVec3_(Vector3& vec, const FileObjectSubNode* vecNode);
        void writeColor_(const Color& vec, FileObjectSubNode* colorNode);
        void writeVec3_(const Vector3& vec, FileObjectSubNode* vecNode);
        void parseFrame_(ParticleSystemFrame& frame, const FileObjectSubNode* frameNode);

        void update_(DelegateParameter& param);
        void render_(DelegateParameter& param);
    public:
        CParticleSystem();
        ~CParticleSystem();

        void OnSharedLoad();

        void OnInit();
        void OnDeinit();

        void Deserialize(const FileObjectSubNode& data);
        void Serialize(FileObjectSubNode& data);
    };
}

#endif
