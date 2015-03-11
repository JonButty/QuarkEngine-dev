/************************************************************************/
/*!
\file		Compositor.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Compositor.h"
#include "Simian/RenderTexture.h"
#include "Simian/Texture.h"
#include "Simian/Material.h"
#include "Simian/DataFileIO.h"
#include "Simian/FileArraySubNode.h"
#include "Simian/LogManager.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/Material.h"
#include "Simian/MaterialCache.h"
#include "Simian/CompositorChain.h"
#include "Simian/Shader.h"

namespace Simian
{
    static std::map<std::string, Simian::u32> createFormatTable_()
    {
        std::map<std::string, Simian::u32> formatTable_;

        formatTable_["RGB"] = Simian::Texture::SF_RGB;
        formatTable_["RGBA"] = Simian::Texture::SF_RGBA;
        formatTable_["RGBAF"] = Simian::Texture::SF_RGBAF;

        return formatTable_;
    }
    static const std::map<std::string, Simian::u32> FORMAT_TABLE = createFormatTable_();


    //--------------------------------------------------------------------------

    Compositor::Pass::Pass()
        : renderTarget_(0),
          material_(0)
    {
    }

    //--------------------------------------------------------------------------

    void Compositor::Pass::Render(CompositorChain* chain)
    {
        // set the render target
        renderTarget_->Use();

        // set all the inputs
        for (Simian::u32 i = 0; i < inputs_.size(); ++i)
            (*inputs_[i])->Set(i);

        // set the material
        material_->Set();

        // do each material pass
        for (Simian::u32 i = 0; i < material_->Size(); ++i)
        {
            // update the shader of the pass if there is one
            if ((*material_)[i].Shader())
                (*material_)[i].Shader()->Update();

            // set the pass draw the quad
            (*material_)[i].Set();
            chain->DrawFullscreenQuad();
            (*material_)[i].Unset();
        }

        // unset
        material_->Unset();

        // unset all inputs
        for (Simian::u32 i = 0; i < inputs_.size(); ++i)
            (*inputs_[i])->Unset();

        renderTarget_->Unuse();
    }

    //--------------------------------------------------------------------------

    Compositor::Compositor( GraphicsDevice* device, MaterialCache* materialCache )
        : previous_(0),
          device_(device),
          output_(0),
          materialCache_(materialCache)
    {
    }

    //--------------------------------------------------------------------------

    RenderTexture* Compositor::Output() const
    {
        return output_;
    }

    //--------------------------------------------------------------------------

    bool Compositor::parseTextures_( const FileArraySubNode* textures )
    {
        for (Simian::u32 i = 0; i < textures->Size(); ++i)
        {
            const FileObjectSubNode* texture = textures->Object(i);

            std::string type;
            texture->Data("Type", type, "");
            if (type != "RenderTarget" && type != "Texture")
            {
                SWarn("Invalid texture type in compositor: " << type);
                continue;
            }

            std::string name;
            texture->Data("Name", name, "");
            if (name == "")
            {
                SWarn("Texture in compositor has no name.");
                continue;
            }

            if (type == "RenderTarget")
            {
                // load the width and height of the compositor
                Simian::f32 width, height;
                std::string formatString;

                texture->Data("Width", width, 0);
                texture->Data("Height", height, 0);
                texture->Data("Format", formatString, "");

                bool autoResize = false;

                if (width == 0 || height == 0)
                {
                    width = device_->Size().X();
                    height = device_->Size().Y();
                    autoResize = true;
                }

                std::map<std::string, Simian::u32>::const_iterator iter = 
                    FORMAT_TABLE.find(formatString);
                if (formatString == "" || iter == FORMAT_TABLE.end())
                {
                    SWarn("Invalid render target: " << name);
                    continue;
                }

                Simian::u32 format = iter->second;

                RenderTexture* texture;
                device_->CreateRenderTexture(texture);
                texture->AutoResize(autoResize);
                if (!texture->LoadImmediate(Simian::Vector2(width, height), format))
                {
                    device_->DestroyRenderTexture(texture);
                    SWarn("Failed to create render texture for compositor: " << name);
                    continue;
                }

                renderTextures_.push_back(texture);
                textureTable_[name] = texture;
            }
            else if (type == "Texture")
            {
                std::string fileName;
                texture->Data("File", fileName, "");

                if (fileName == "")
                {
                    SWarn("Texture filename in compositor is empty.");
                    continue;
                }

                Texture* texture;
                device_->CreateTexture(texture);
                if (!texture->LoadImmediate(fileName))
                {
                    device_->DestroyTexture(texture);
                    SWarn("Failed to create texture for compositor: " << fileName);
                    continue;
                }

                textures_.push_back(texture);
                textureTable_[name] = texture;
            }
        }

        return true;
    }

    bool Compositor::createPasses_( const FileArraySubNode* passes )
    {
        for (Simian::u32 i = 0; i < passes->Size(); ++i)
        {
            const FileObjectSubNode* passNode = passes->Object(i);
            std::string passName;
            passNode->Data("Name", passName, "");

            if (passName == "")
            {
                SWarn("Pass in compositor does not have a name.");
                continue;
            }

            std::map<std::string, Pass*>::iterator iter = passTable_.find(passName);
            if (iter != passTable_.end())
            {
                SWarn("Pass with name: " << passName << " already exists in compositor.");
                continue;
            }

            Pass* pass = new Pass();
            passTable_[passName] = pass;
            passes_.push_back(pass);
        }
        return true;
    }

    bool Compositor::initPasses_( const FileArraySubNode* passes )
    {
        unsigned int j = 0;
        for (Simian::u32 i = 0; i < passes->Size(); ++i)
        {
            const FileObjectSubNode* pass = passes->Object(i);
            std::string name;
            pass->Data("Name", name, "");

            // no pass name, not valid
            if (name == "")
                continue;

            // valid use the pass ptr
            parsePass_(passes_[j++], pass);
        }
        return true;
    }

    void Compositor::parsePass_( Pass* pass, const FileObjectSubNode* passNode )
    {
        // create all the properties for a pass
        std::string name;
        passNode->Data("Name", name, "OutputPass");
        pass->name_ = name;

        // render target
        std::string renderTarget;
        passNode->Data("RenderTarget", renderTarget, "");
        if (renderTarget != "")
        {
            std::map<std::string, TextureBase*>::iterator renderIter = textureTable_.find(renderTarget);
            if (renderIter == textureTable_.end())
            {
                SWarn("No such render target: " << renderTarget);
                return;
            }
            pass->renderTarget_ = reinterpret_cast<RenderTexture*>(renderIter->second);
        }
        
        // resolve inputs
        const FileArraySubNode* input = passNode->Array("Input");
        if (input)
        {
            for (Simian::u32 i = 0; i < input->Size(); ++i)
            {
                const FileDataSubNode* nameNode = input->Data(i);
                std::string name = nameNode->AsString();

                // first check if its previous
                if (name == "previous" || name == "Previous")
                {
                    pass->inputs_.push_back(&previous_);
                    continue;
                }

                // find on the texture table
                std::map<std::string, TextureBase*>::iterator texiter = textureTable_.find(name);
                if (texiter != textureTable_.end())
                {
                    pass->inputs_.push_back(&texiter->second);
                    continue;
                }

                // find on the pass table
                std::map<std::string, Pass*>::iterator iter = passTable_.find(name);
                if (iter == passTable_.end())
                {
                    SWarn("Could not find pass: " << name << " in pass table.");
                    continue;
                }
                pass->inputs_.push_back(
                    reinterpret_cast<TextureBase**>(&iter->second->renderTarget_));
            }
        }

        // get material file name
        std::string materialFile;
        passNode->Data("MaterialFile", materialFile, "");
        if (materialFile == "")
        {
            SWarn("No material in compositor pass: " << name);
            return;
        }
        pass->material_ = materialCache_->Load(materialFile);
    }

    //--------------------------------------------------------------------------

    bool Compositor::Load( const DataLocation& location )
    {
        DataFileIODefault file;
        file.Read(location);

        // create textures
        const FileArraySubNode* textures = file.Root()->Array("Textures");

        if (textures && !parseTextures_(textures))
        {
            Unload();
            return false;
        }

        const FileArraySubNode* passes = file.Root()->Array("Passes");

        if (passes && !createPasses_(passes))
        {
            Unload();
            return false;
        }

        if (passes && !initPasses_(passes))
        {
            Unload();
            return false;
        }

        const FileObjectSubNode* outputPass = file.Root()->Object("OutputPass");
        if (!outputPass)
        {
            SWarn("Compositor: " << location.Identifier() << " has no output pass.");
            Unload();
            return false;
        }

        Pass* pass = new Pass();
        parsePass_(pass, outputPass);
        if (!pass)
        {
            delete pass;
            Unload();
            return false;
        }
        passes_.push_back(pass);

        // create the rendertexture
        device_->CreateRenderTexture(output_);
        output_->LoadImmediate(device_->Size(), Simian::Texture::SF_RGBA);
        output_->AutoResize(true);

        pass->renderTarget_ = output_;

        return true;
    }

    void Compositor::Unload()
    {
        if (output_)
        {
            output_->Unload();
            device_->DestroyRenderTexture(output_);
        }

        // destroy all passes
        for (Simian::u32 i = 0; i < passes_.size(); ++i)
            delete passes_[i];
        passes_.clear();
        passTable_.clear();

        // destroy all textures
        for (Simian::u32 i = 0; i < textures_.size(); ++i)
        {
            textures_[i]->Unload();
            device_->DestroyTexture(textures_[i]);
        }
        textures_.clear();

        // destroy render textures
        for (Simian::u32 i = 0; i < renderTextures_.size(); ++i)
        {
            renderTextures_[i]->Unload();
            device_->DestroyRenderTexture(renderTextures_[i]);
        }
        renderTextures_.clear();

        textureTable_.clear();
    }

    void Compositor::Render()
    {
        // render each pass
        for (Simian::u32 i = 0; i < passes_.size(); ++i)
            passes_[i]->Render(chain_);
    }
}
