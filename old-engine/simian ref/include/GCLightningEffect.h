/*************************************************************************/
/*!
\file		GCLightningEffect.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCLIGHTNINGEFFECT_H_
#define DESCENSION_GCLIGHTNINGEFFECT_H_

#include "Simian/EntityComponent.h"
#include "Simian/Vector3.h"
#include "Simian/VertexBuffer.h"

#include <vector>
#include <string>

namespace Simian
{
    class IndexBuffer;
    class Camera;
    class Material;
    class CTransform;
}

namespace Descension
{
    class GCLightningEffect: public Simian::EntityComponent
    {
    private:
        struct LightningSegment
        {
            Simian::Vector3 From;
            Simian::Vector3 FromRight;
            Simian::Vector3 To;
            Simian::Vector3 ToRight;
            Simian::Color Color;
        };
    private:
        Simian::CTransform* transform_;
        Simian::VertexBuffer* vertexBuffer_;
        Simian::IndexBuffer* indexBuffer_;

        Simian::u32 generations_;
        Simian::f32 deviation_;
        Simian::f32 thickness_;
        Simian::f32 flickerDuration_;
        Simian::f32 timer_;
        Simian::f32 forkChance_;
        Simian::f32 forkScale_;
        Simian::f32 deviationDecay_;
        Simian::f32 colorDecay_;
        std::vector<LightningSegment> lightningPoints_;
        std::vector<Simian::PositionColorUVVertex> vertices_;

        Simian::Vector3 origin_, target_;
        Simian::Camera* camera_;
        Simian::u8 layer_;
        std::string materialFile_;
        Simian::Material* material_;

        bool visible_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCLightningEffect> factoryPlant_;
    public:
        const Simian::Vector3& Origin() const;
        void Origin(const Simian::Vector3& val);

        const Simian::Vector3& Target() const;
        void Target(const Simian::Vector3& val);

        bool Visible() const;
        void Visible(bool val);
    private:
        void update_(Simian::DelegateParameter& param);
        void render_(Simian::DelegateParameter& param);
        void draw_(Simian::DelegateParameter& param);

        void generatePoints_();
        void updateVertices_();
    public:
        GCLightningEffect();

        void OnSharedLoad();
        void OnAwake();
        void OnDeinit();

        void Serialize( Simian::FileObjectSubNode& );
        void Deserialize( const Simian::FileObjectSubNode& );
    };
}

#endif
