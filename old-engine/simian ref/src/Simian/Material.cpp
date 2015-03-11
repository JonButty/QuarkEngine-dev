/************************************************************************/
/*!
\file		Material.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Material.h"
#include "Simian/DataFileIO.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/LogManager.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/TextureCache.h"
#include "Simian/Texture.h"

#include <map>
#include "Simian/Shader.h"
#include "Simian/ShaderCache.h"

namespace Simian
{
    std::map<std::string, RenderTexture*> Material::renderTextures_;

    std::map<std::string, u32> generateRenderStateTable_()
    {
        std::map<std::string, u32> out;
        out["cullmode"]     = GraphicsDevice::RS_CULLING;
        out["blending"]     = GraphicsDevice::RS_BLENDING;
        out["depthenabled"] = GraphicsDevice::RS_DEPTHENABLED;
        out["depthwrite"]   = GraphicsDevice::RS_DEPTHWRITE;
        out["lighting"]     = GraphicsDevice::RS_LIGHTING;
        out["srcblend"]     = GraphicsDevice::RS_SRCBLEND;
        out["dstblend"]     = GraphicsDevice::RS_DSTBLEND;
        out["alphatest"]    = GraphicsDevice::RS_ALPHATEST;
        out["alpharef"]    = GraphicsDevice::RS_ALPHAREF;
        out["alphafunc"]    = GraphicsDevice::RS_ALPHAFUNC;

        return out;
    }
    static const std::map<std::string, u32> RENDERSTATE_NAME_TABLE(generateRenderStateTable_());

    std::map<std::string, u32> generateRenderStateValTable_()
    {
        std::map<std::string, u32> out;
        out["cw"] = GraphicsDevice::SCM_CW;
        out["ccw"] = GraphicsDevice::SCM_CCW;
        out["none"] = GraphicsDevice::SCM_NONE;

        out["zero"] = GraphicsDevice::BM_ZERO;
        out["one"] = GraphicsDevice::BM_ONE;
        out["srcalpha"] = GraphicsDevice::BM_SRCALPHA;
        out["invsrcalpha"] = GraphicsDevice::BM_INVSRCALPHA;

        out["true"] = 1;
        out["false"] = 0;

        out["comparealways"] = GraphicsDevice::AF_ALWAYS;
        out["comparenever"] = GraphicsDevice::AF_NEVER;
        out["comparegreater"] = GraphicsDevice::AF_GREATER;
        out["compareless"] = GraphicsDevice::AF_LESS;

        return out;
    }
    static const std::map<std::string, u32> RENDERSTATE_VALUE_TABLE(generateRenderStateValTable_());

    std::map<std::string, u32> generateSamplerStateTable_()
    {
        std::map<std::string, u32> out;
        out["magfilter"] = Texture::SS_MAGFILTER;
        out["minfilter"] = Texture::SS_MINFILTER;
        out["wrapu"] = Texture::SS_WRAPU;
        out["wrapv"] = Texture::SS_WRAPV;

        return out;
    }
    static const std::map<std::string, u32> SAMPLERSTATE_NAME_TABLE(generateSamplerStateTable_());

    std::map<std::string, u32> generateSamplerStateValTable_()
    {
        std::map<std::string, u32> out;
        out["point"] = Texture::FF_POINT;
        out["linear"] = Texture::FF_LINEAR;
        out["bilinear"] = Texture::FF_BILINEAR;
        out["trilinear"] = Texture::FF_TRILINEAR;

        out["clamp"] = Texture::WF_CLAMP;
        out["repeat"] = Texture::WF_REPEAT;

        return out;
    }
    static const std::map<std::string, u32> SAMPLERSTATE_VALUE_TABLE(generateSamplerStateValTable_());

    std::map<std::string, u32> generateTextureStateTable_()
    {
        std::map<std::string, u32> out;
        out["colorop"] = Texture::TSF_COLOROP;
        out["colorarg1"] = Texture::TSF_COLORARG1;
        out["colorarg2"] = Texture::TSF_COLORARG2;
        out["alphaop"] = Texture::TSF_ALPHAOP;
        out["alphaarg1"] = Texture::TSF_ALPHAARG1;
        out["alphaarg2"] = Texture::TSF_ALPHAARG2;
        out["texcoordindex"] = Texture::TSF_TEXCOORDINDEX;
        out["transformflag"] = Texture::TSF_TRANSFORMFLAG;
        out["constant"] = Texture::TSF_CONSTANT;

        return out;
    }
    static const std::map<std::string, u32> TEXTURESTATE_NAME_TABLE(generateTextureStateTable_());

    std::map<std::string, u32> generateTextureStateValTable_()
    {
        std::map<std::string, u32> out;
        out["nooperation"] = Texture::TSO_DISABLED;
        out["choosearg1"] = Texture::TSO_ARG1;
        out["choosearg2"] = Texture::TSO_ARG2;
        out["modulate"] = Texture::TSO_MODULATE;
        out["modulate2x"] = Texture::TSO_MODULATE2X;
        out["modulate4x"] = Texture::TSO_MODULATE4X;
        out["add"] = Texture::TSO_ADD;

        out["constantcolor"] = Texture::TSA_CONSTANT;
        out["currentcolor"] = Texture::TSA_CURRENT;
        out["diffusecolor"] = Texture::TSA_DIFFUSE;
        out["texturecolor"] = Texture::TSA_TEXTURE;

        out["transformdisabled"] = Texture::TTF_DISABLED;
        out["transformcount1"] = Texture::TTF_COUNT1;
        out["transformcount2"] = Texture::TTF_COUNT2;
        out["transformcount3"] = Texture::TTF_COUNT3;
        out["transformcount1proj"] = Texture::TTF_COUNT1PROJ;
        out["transformcount2proj"] = Texture::TTF_COUNT2PROJ;
        out["transformcount3proj"] = Texture::TTF_COUNT3PROJ;

        out["cameraposition"] = Texture::TCI_CAMERAPOSITION;

        return out;
    }
    static const std::map<std::string, u32> TEXTURESTATE_VALUE_TABLE(generateTextureStateValTable_());

    std::map<std::string, u32> generateSurfaceFormatTable()
    {
        std::map<std::string, u32> out;
        out["RGBA"] = Texture::SF_RGBA;
        out["RGB"] = Texture::SF_RGB;
        out["RGBAF"] = Texture::SF_RGBAF;
        return out;
    }
    static const std::map<std::string, u32> SURFACEFORMAT_TABLE(generateSurfaceFormatTable());

    Material::Material(GraphicsDevice* device, TextureCache* cache, ShaderCache* shaderCache, u32 id)
        : device_(device),
          textureCache_(cache),
          shaderCache_(shaderCache),
          id_(id),
          alphaBlended_(false)
    {
    }

    //--------------------------------------------------------------------------

    GraphicsDevice& Material::Device() const
    {
        return *device_;
    }

    Simian::u32 Material::Id() const
    {
        return id_;
    }

    Material::PassIterator Material::Begin()
    {
        return passes_.begin();
    }

    Material::PassIterator Material::End()
    {
        return passes_.end();
    }

    Simian::u32 Material::Size() const
    {
        return passes_.size();
    }

    bool Material::AlphaBlended() const
    {
        return alphaBlended_;
    }

    std::map<std::string, RenderTexture*>& Material::RenderTextures()
    {
        return renderTextures_;
    }

    //--------------------------------------------------------------------------

    Material::Pass& Material::operator[]( u32 index )
    {
        return passes_[index];
    }

    const Material::Pass& Material::operator[]( u32 index ) const
    {
        return passes_[index];
    }

    //--------------------------------------------------------------------------

    bool Material::parseRenderState_( FileDataSubNode* dataNode )
    {
        std::map<std::string, u32>::const_iterator iter = 
            RENDERSTATE_NAME_TABLE.find(SToLower(dataNode->Name()));
        u32 key, value;

        // no such render state
        if (iter == RENDERSTATE_NAME_TABLE.end())
        {
            SWarn(std::string("No such render state in material: ") + dataNode->Name());
            return false;
        }

        key = iter->second;

        // check for a value
        iter = RENDERSTATE_VALUE_TABLE.find(SToLower(dataNode->AsString()));
        if (iter != RENDERSTATE_VALUE_TABLE.end())
            value = iter->second;
        else if (dataNode->IsFloat())
            value = static_cast<u32>(dataNode->AsF32());
        else
        {
            SWarn(std::string("Invalid value for ") + dataNode->Name() + " :" + dataNode->AsString());
            return false;
        }

        AddRenderState(key, value);

        return true;
    }

    bool Material::parsePasses_( FileArraySubNode* node )
    {
        FileArraySubNode& arrayNode = *node;
        for (u32 j = 0; j < arrayNode.Size(); ++j)
        {
            if (arrayNode[j]->Type() == FileArraySubNode::FNT_OBJECT)
                parsePass_(reinterpret_cast<FileObjectSubNode*>(arrayNode[j]));
        }
        return true;
    }

    bool Material::parsePass_( FileObjectSubNode* node )
    {
        FileObjectSubNode& object = *node;
        Pass pass(this);

        for (FileObjectSubNode::Iterator i = object.Begin(); i != object.End(); ++i)
        {
            if ((*i)->Type() == FileObjectSubNode::FNT_DATA)
            {
                FileDataSubNode* data = reinterpret_cast<FileDataSubNode*>(*i);
                u32 key, value;
                const std::map<std::string, u32>* valueTable = 0;

                // see if it is a render state
                std::map<std::string, u32>::const_iterator iter = RENDERSTATE_NAME_TABLE.find(SToLower(data->Name()));
                if (iter != RENDERSTATE_NAME_TABLE.end())
                {
                    valueTable = &RENDERSTATE_VALUE_TABLE;
                }
                else
                {
                    // not a render state, see if its a sampler state
                    iter = SAMPLERSTATE_NAME_TABLE.find(SToLower(data->Name()));
                    if (iter == SAMPLERSTATE_NAME_TABLE.end())
                    {
                        SWarn(std::string("No such sampler state in material: ") + data->Name());
                        continue;
                    }
                    else
                        valueTable = &SAMPLERSTATE_VALUE_TABLE;
                }

                key = iter->second;
                
                iter = valueTable->find(SToLower(data->AsString()));
                if (iter != valueTable->end())
                    value = iter->second;
                else if (data->IsFloat())
                    value = static_cast<u32>(data->AsF32());
                else
                {
                    SWarn(std::string("Invalid value for ") + data->Name() + " :" + data->AsString());
                    continue;
                }

                if (valueTable == &RENDERSTATE_VALUE_TABLE)
                    pass.AddRenderState(key, value);
                else
                    pass.AddSamplerState(key, value);
            }
            else if ((*i)->Type() == FileObjectSubNode::FNT_ARRAY && SToLower((*i)->Name()) == "textures")
            {
                parseTextureStages_(reinterpret_cast<FileArraySubNode*>(*i), pass);
            }
            else if ((*i)->Type() == FileObjectSubNode::FNT_OBJECT && SToLower((*i)->Name()) == "shader")
            {
                parseShader_(reinterpret_cast<FileObjectSubNode*>(*i), pass);
            }
        }

        passes_.push_back(pass);
        return true;
    }

    bool Material::parseTextureStages_( FileArraySubNode* node, Pass& pass )
    {
        FileArraySubNode& arrayNode = *node;
        for (u32 j = 0; j < arrayNode.Size(); ++j)
        {
            if (arrayNode[j]->Type() == FileArraySubNode::FNT_OBJECT)
            {
                FileObjectSubNode* textureStage = arrayNode.Object(j);

                // create the texture state object
                TextureStage stage(this);

                // get the texture of this texture stage
                FileDataSubNode* texture = textureStage->Data("Texture");

                // look for the dynamic property
                FileObjectSubNode* dynamic = textureStage->Object("Dynamic");
                
                if (dynamic)
                {
                    // load fields
                    u32 width, height, levels;
                    std::string format;

                    dynamic->Data("Width", width);
                    dynamic->Data("Height", height);
                    dynamic->Data("Levels", levels, 1U);
                    dynamic->Data("Format", format, "RGB");

                    if (!width || !height)
                        return false;

                    std::map<std::string, u32>::const_iterator i = SURFACEFORMAT_TABLE.find(format);
                    if (i == SURFACEFORMAT_TABLE.end())
                        return false;

                    // create a new texture from scratch
                    Texture* texture;
                    device_->CreateTexture(texture);
                    if (!texture->CreateTexture(width, height, i->second, levels, true))
                    {
                        device_->DestroyTexture(texture);
                        return false;
                    }

                    stage.Texture(texture);
                }
                else if (texture)
                    stage.Texture(textureCache_->Load(texture->AsString()));
                else
                {
                    texture = textureStage->Data("RenderTexture");
                    stage.RenderTexture(&renderTextures_[texture->AsString()]);
                }

                // load the rest of the properties
                for (FileObjectSubNode::Iterator i = textureStage->Begin(); 
                    i != textureStage->End(); ++i)
                {
                    if ((*i)->Type() != FileDataSubNode::FNT_DATA)
                        continue;

                    FileDataSubNode& data = *reinterpret_cast<FileDataSubNode*>(*i);
                    if (data.Name() == "Texture" || data.Name() == "RenderTexture"
                        || data.Name() == "Dynamic")
                        continue;

                    std::map<std::string, u32>::const_iterator iter;
                    u32 key, value;

                    iter = TEXTURESTATE_NAME_TABLE.find(SToLower(data.Name()));
                    if (iter == TEXTURESTATE_NAME_TABLE.end())
                    {
                        SWarn("No such texture state: " + data.Name());
                        continue;
                    }

                    key = iter->second;

                    iter = TEXTURESTATE_VALUE_TABLE.find(SToLower(data.AsString()));
                    if (iter != TEXTURESTATE_VALUE_TABLE.end())
                        value = iter->second;
                    else if (data.IsFloat())
                        value = data.AsU32();
                    else
                    {
                        SWarn(std::string("Invalid value for ") + data.Name() + " :" + data.AsString());
                        continue;
                    }

                    stage.AddTextureStageState(key, value);
                }

                pass.AddTextureStage(stage);
            }
        }

        return true;
    }

    bool Material::parseShader_( FileObjectSubNode* node, Pass& pass )
    {
        const FileArraySubNode* vertexProgramConstants = node->Array("VertexProgramConstants");
        const FileArraySubNode* fragmentProgramConstants = node->Array("FragmentProgramConstants");

        std::string file;
        node->Data("File", file, "");

        if (file == "")
            return false;

        Shader* shader;

        // we must clone the shader if it has custom properties
        if (vertexProgramConstants || fragmentProgramConstants)
        {
            shader = new Shader(device_);
            if (!shader->Load<DataFileIODefault>(file))
            {
                delete shader;
                return false;
            }
            if (vertexProgramConstants)
                shader->LoadVertexProgramConstants(*vertexProgramConstants);
            if (fragmentProgramConstants)
                shader->LoadFragmentProgramConstants(*fragmentProgramConstants);
            pass.shaderCloned_ = true;
        }
        else
            shader = shaderCache_->Load(file);

        pass.shader_ = shader;

        return true;
    }

    //--------------------------------------------------------------------------

    bool Material::Load( const DataFileIO& document )
    {
        FileRootSubNode* root = document.Root();

        for (FileRootSubNode::Iterator i = root->Begin(); i != root->End(); ++i)
        {
            if ((*i)->Type() == FileRootSubNode::FNT_DATA)
                parseRenderState_(reinterpret_cast<FileDataSubNode*>(*i));
            else if (SToLower((*i)->Name()) == "passes" && (*i)->Type() == FileRootSubNode::FNT_ARRAY)
                parsePasses_(reinterpret_cast<FileArraySubNode*>(*i));
        }

        return true;
    }

    void Material::Unload()
    {
        for (u32 i = 0; i < passes_.size(); ++i)
            passes_[i].Unload();
        passes_.clear();
        renderStates_.clear();
    }

    void Material::Set()
    {
        // set the device states
        for (u32 i = 0; i < renderStates_.size(); ++i)
        {
            renderStates_[i].second.Previous = device_->RenderState(renderStates_[i].first);
            device_->RenderState(renderStates_[i].first, renderStates_[i].second.Current);
        }
    }

    void Material::Unset()
    {
        // change render states back!
        for (u32 i = 0; i < renderStates_.size(); ++i)
            device_->RenderState(renderStates_[i].first, renderStates_[i].second.Previous);
    }

    void Material::AddRenderState( u32 renderState, u32 value )
    {
        CurrentPrevPair cppair;
        cppair.Previous = 0;
        cppair.Current = value;
        renderStates_.push_back(std::make_pair(renderState, cppair));
        alphaBlended_ = renderState == GraphicsDevice::RS_BLENDING && value ? 
            true : alphaBlended_;
    }
}
