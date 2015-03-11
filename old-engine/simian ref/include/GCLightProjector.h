/*************************************************************************/
/*!
\file		GCLightProjector.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCLIGHTPROJECTOR_H_
#define DESCENSION_GCLIGHTPROJECTOR_H_

#include "Simian/EntityComponent.h"
#include "Simian/Color.h"

namespace Simian
{
    class CTransform;
    class COrthoCamera;
    class VertexBuffer;
    class Material;
}

namespace Descension
{
    class GCLightProjector: public Simian::EntityComponent
    {
    private:
        Simian::CTransform* transform_;
        Simian::CTransform* cameraTransform_;
        Simian::COrthoCamera* projector_;
        Simian::Color ambient_;
        Simian::Color edge_;
        Simian::f32 projectionSize_;
        Simian::u32 textureMatrixIndex_;
        Simian::VertexBuffer* unitQuad_;
        Simian::Material* edgeMaterial_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCLightProjector> factoryPlant_;
    public:
        const Simian::Color& Ambient() const;
        void Ambient(const Simian::Color& val);

        const Simian::Color& Edge() const;
        void Edge(const Simian::Color& val);
    private:
        void updateTextureMatrix_(Simian::DelegateParameter& param);
        void drawEdges_(Simian::DelegateParameter& param);
        void render_(Simian::DelegateParameter& param);
    public:
        GCLightProjector();

        void OnSharedLoad();
        void OnAwake();
        void OnDeinit();

        void Deserialize(const Simian::FileObjectSubNode& data);
        void Serialize(Simian::FileObjectSubNode& data);
    };
}

#endif
