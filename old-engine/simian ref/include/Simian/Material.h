/************************************************************************/
/*!
\file		Material.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_MATERIAL_H_
#define SIMIAN_MATERIAL_H_

#include "SimianPlatform.h"

#include "Simian/DataLocation.h"

#include <vector>
#include <xutility>
#include <map>

namespace Simian
{
    class Texture;
    class RenderTexture;
    class DataFileIO;
    class GraphicsDevice;
    class TextureCache;
    class Shader;
    class ShaderCache;

    class FileDataSubNode;
    class FileObjectSubNode;
    class FileArraySubNode;

    class SIMIAN_EXPORT Material
    {
    public:
        class SIMIAN_EXPORT TextureStage
        {
        private:
            struct CurrentPrevPair
            {
                u32 Current;
                u32 Previous;
            };
            typedef std::pair<u32, CurrentPrevPair> TextureStageStatePair;
        private:
            Material* material_;
            Simian::Texture* texture_;
            Simian::RenderTexture** renderTexture_;
            std::vector<TextureStageStatePair> textureStates_;
        public:
            Simian::Texture* Texture() const;
            void Texture(Simian::Texture* val);

            Simian::RenderTexture** RenderTexture() const;
            void RenderTexture(Simian::RenderTexture** val);
        public:
            TextureStage(Material* material);
            ~TextureStage();

            void AddTextureStageState(u32 key, u32 value);

            void Set(u32 sampler);
            void Unset();

            void Unload();
        };

        class SIMIAN_EXPORT Pass
        {
        private:
            struct CurrentPrevPair
            {
                u32 Current;
                u32 Previous;
            };
            typedef std::pair<u32, u32> SamplerStatePair;
            typedef std::pair<u32, CurrentPrevPair> RenderStatePair;
        private:
            Material* parent_;
            Shader* shader_;
            bool shaderCloned_;
            
            // TODO: vertex/pixel shader in here too
            std::vector<SamplerStatePair> samplerStates_;
            std::vector<RenderStatePair> renderStates_;
            std::vector<TextureStage> textureStages_;
        public:
            const std::vector<TextureStage>& TextureStages() const;
            Simian::Shader* Shader() const;
        public:
            Pass(Material* parent);
            ~Pass();

            void Set();
            void Unset();

            void AddRenderState(u32 renderState, u32 value);
            void AddSamplerState(u32 samplerState, u32 value);
            void AddTextureStage(const TextureStage& texture);

            void Unload();

            friend class Material;
        };
        typedef std::vector<Pass>::iterator PassIterator;
    private:
        struct CurrentPrevPair
        {
            u32 Current;
            u32 Previous;
        };
        typedef std::pair<u32, CurrentPrevPair> RenderStatePair;
    private:
        GraphicsDevice* device_;
        TextureCache* textureCache_;
        ShaderCache* shaderCache_;
        u32 id_;
        std::vector<RenderStatePair> renderStates_;
        std::vector<Pass> passes_;
        bool alphaBlended_;

        static std::map<std::string, RenderTexture*> renderTextures_;
    public:
        GraphicsDevice& Device() const;
        Simian::u32 Id() const;
        PassIterator Begin();
        PassIterator End();
        u32 Size() const;
        bool AlphaBlended() const;

        static std::map<std::string, RenderTexture*>& RenderTextures();
    public:
        Pass& operator[](u32 index);
        const Pass& operator[](u32 index) const;
    private:
        bool parseRenderState_(FileDataSubNode* node);
        bool parseTextureStages_(FileArraySubNode* node, Pass& pass);
        bool parseShader_(FileObjectSubNode* node, Pass& pass);
        bool parsePasses_(FileArraySubNode* node);
        bool parsePass_(FileObjectSubNode* node);
    public:
        Material(GraphicsDevice* device, TextureCache* textureCache, ShaderCache* shaderCache, u32 id);

        template <class T> // type of file loader to use
        bool Load(const DataLocation& path)
        {
            T reader;
            if (!reader.Read(path))
                return false;
            return Load(reader);
        }

        bool Load(const DataFileIO& document);
        void Unload();

        void Set();
        void Unset();

        void AddRenderState(u32 renderState, u32 value);

        friend class Material::Pass;
    };
}

#endif
