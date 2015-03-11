/************************************************************************/
/*!
\file		CSprite.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CSPRITE_H_
#define SIMIAN_CSPRITE_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"
#include "Vector2.h"
#include "Color.h"

namespace Simian
{
    class CTransform;
    class VertexBuffer;
    class Material;
    class RenderQueue;
    class GraphicsDevice;
    class MaterialCache;

    class SIMIAN_EXPORT CSprite: public EntityComponent
    {
    private:
        CTransform* transform_;

        VertexBuffer* vertexBuffer_;
        Material* material_;
        std::string materialLocation_;
        Vector2 size_;
        Simian::Color color_;
        u8 layer_;
        std::vector<u32> oldStates_;
        bool visible_;

        static RenderQueue* renderQueue_;
        static GraphicsDevice* graphicsDevice_;
        static MaterialCache* materialCache_;

        static FactoryPlant<EntityComponent, CSprite> factoryPlant_;
    public:
        static Simian::RenderQueue* RenderQueue();
        static void RenderQueue(Simian::RenderQueue* val);

        static Simian::GraphicsDevice* GraphicsDevice();
        static void GraphicsDevice(Simian::GraphicsDevice* val);

        static Simian::MaterialCache* MaterialCache();
        static void MaterialCache(Simian::MaterialCache* val);

        const Simian::Vector2& Size() const;
        void Size(const Simian::Vector2& val);

        const Simian::Color& Color() const;
        void Color(const Simian::Color& val);

        Simian::Material* Material() const;
        void Material(Simian::Material* val);

        bool Visible() const;
        void Visible(bool val);
    private:
        void render_(DelegateParameter& param);
        void draw_(DelegateParameter& param);
    public:
        CSprite();

        void OnSharedLoad();
        void OnSharedUnload();
        void OnAwake();

        void Serialize( FileObjectSubNode& );
        void Deserialize( const FileObjectSubNode& );
    };
}

#endif
