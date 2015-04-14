/************************************************************************/
/*!
\file		Compositor.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_COMPOSITOR_H_
#define SIMIAN_COMPOSITOR_H_

#include "SimianPlatform.h"
#include "DataLocation.h"

#include <map>
#include <string>
#include <vector>

namespace Simian
{
    class RenderTexture;
    class TextureBase;
    class Material;
    class FileArraySubNode;
    class GraphicsDevice;
    class Texture;
    class FileObjectSubNode;
    class MaterialCache;
    class CompositorChain;

    class SIMIAN_EXPORT Compositor
    {
    public:
        class Pass
        {
        private:
            std::string name_;
            RenderTexture* renderTarget_;
            std::vector<TextureBase**> inputs_;
            Material* material_;
        public:
            Pass();

            void Render(CompositorChain* chain);

            friend class Compositor;
        };
    private:
        CompositorChain* chain_;

        std::map<std::string, TextureBase*> textureTable_;
        std::map<std::string, Pass*> passTable_;

        std::vector<RenderTexture*> renderTextures_;
        std::vector<Texture*> textures_;
        std::vector<Pass*> passes_;

        TextureBase* previous_;
        GraphicsDevice* device_;
        RenderTexture* output_;
        MaterialCache* materialCache_;
    public:
        RenderTexture* Output() const;
    private:
        bool parseTextures_(const FileArraySubNode* textures);
        bool createPasses_(const FileArraySubNode* passes);
        void parsePass_(Pass* pass, const FileObjectSubNode* passNode);
        bool initPasses_(const FileArraySubNode* passes);
    public:
        Compositor(GraphicsDevice* device, MaterialCache* materialCache);

        bool Load(const DataLocation& location);
        void Unload();
        void Render();

        friend class CompositorChain;
    };
}

#endif
