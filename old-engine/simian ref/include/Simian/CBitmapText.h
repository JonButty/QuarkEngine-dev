/************************************************************************/
/*!
\file		CBitmapText.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CBITMAPTEXT_H_
#define SIMIAN_CBITMAPTEXT_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

#include <string>

namespace Simian
{
    class Material;
    class BitmapFont;
    class BitmapText;
    class GraphicsDevice;
    class RenderQueue;
    class CTransform;
    class MaterialCache;

    class SIMIAN_EXPORT CBitmapText: public EntityComponent
    {
    private:
        Material* fontMaterial_;
        BitmapFont* bitmapFont_;
        BitmapText* bitmapText_;

        u32 cacheSize_;
        std::string fontFile_;
        std::string materialFile_;
        std::string text_;
        Simian::Vector2 alignment_;
        Simian::Vector2 scale_;
        f32 opacity_;
        u8 layer_;

        CTransform* transform_;

        static Simian::GraphicsDevice* graphicsDevice_;
        static Simian::RenderQueue* renderQueue_;
        static Simian::MaterialCache* materialCache_;
        static FactoryPlant<EntityComponent, CBitmapText> factoryPlant_;
    public:
        const std::string& Text() const;
        void Text(const std::string& val);

        static Simian::GraphicsDevice* GraphicsDevice();
        static void GraphicsDevice(Simian::GraphicsDevice* val);

        static Simian::RenderQueue* RenderQueue();
        static void RenderQueue(Simian::RenderQueue* val);

        static Simian::MaterialCache* MaterialCache();
        static void MaterialCache(Simian::MaterialCache* val);

        Simian::f32 Opacity() const;
        void Opacity(Simian::f32 val);
    private:
        void updateText_();

        void update_(Simian::DelegateParameter&);
        void render_(Simian::DelegateParameter&);
        void draw_(Simian::DelegateParameter&);
    public:
        CBitmapText();

        void OnSharedLoad();
        void OnSharedUnload();
        void OnAwake();
        void OnDeinit();

        void Serialize( FileObjectSubNode& );
        void Deserialize( const FileObjectSubNode& );
    };
}

#endif
