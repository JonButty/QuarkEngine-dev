/************************************************************************/
/*!
\file		CBitmapText.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CBitmapText.h"
#include "Simian/EngineComponents.h"
#include "Simian/EnginePhases.h"
#include "Simian/BitmapFont.h"
#include "Simian/BitmapText.h"
#include "Simian/CCamera.h"
#include "Simian/Camera.h"
#include "Simian/CTransform.h"
#include "Simian/RenderQueue.h"
#include "Simian/MaterialCache.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CBitmapText> CBitmapText::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_BITMAPTEXT);
    Simian::GraphicsDevice* CBitmapText::graphicsDevice_ = 0;
    Simian::RenderQueue* CBitmapText::renderQueue_ = 0;
    Simian::MaterialCache* CBitmapText::materialCache_ = 0;

    CBitmapText::CBitmapText()
        : fontMaterial_(0),
          bitmapFont_(0),
          bitmapText_(0),
          cacheSize_(1024),
          scale_(1.0f, 1.0f),
          opacity_(1.0f),
          layer_(RenderQueue::RL_SCENE),
          transform_(0)
    {
    }

    //--------------------------------------------------------------------------

    const std::string& CBitmapText::Text() const
    {
        return text_;
    }

    void CBitmapText::Text( const std::string& val )
    {
        text_ = val;
        if (bitmapText_)
            updateText_();
    }

    Simian::GraphicsDevice* CBitmapText::GraphicsDevice()
    {
        return graphicsDevice_;
    }

    void CBitmapText::GraphicsDevice( Simian::GraphicsDevice* val )
    {
        graphicsDevice_ = val;
    }

    Simian::RenderQueue* CBitmapText::RenderQueue()
    {
        return renderQueue_;
    }

    void CBitmapText::RenderQueue( Simian::RenderQueue* val )
    {
        renderQueue_ = val;
    }

    Simian::MaterialCache* CBitmapText::MaterialCache()
    {
        return materialCache_;
    }

    void CBitmapText::MaterialCache( Simian::MaterialCache* val )
    {
        materialCache_ = val;
    }

    Simian::f32 CBitmapText::Opacity() const
    {
        return opacity_;
    }

    void CBitmapText::Opacity( Simian::f32 val )
    {
        opacity_ = val;
        if (bitmapText_)
            bitmapText_->Opacity(opacity_);
    }

    //--------------------------------------------------------------------------

    void CBitmapText::updateText_()
    {
        std::string final = "";

        bitmapText_->ClearColors();

        // parse text for color codes
        u32 prevPos = 0;
        u32 pos = 0;
        u32 acc = 0;
        while ((pos = text_.find_first_of('^', pos)) != std::string::npos)
        {
            final += text_.substr(prevPos, pos - prevPos);

            Simian::Color color;
            char code = text_[pos + 1];

            switch (code)
            {
            case 'r':
                color = Simian::Color(1.0f, 0.0f, 0.0f, 1.0f);
                break;
            case 'g':
                color = Simian::Color(0.0f, 0.85f, 0.0f, 1.0f);
                break;
            case 'b':
                color = Simian::Color(0.4f, 0.4f, 1.0f, 1.0f);
                break;
            case 'w':
                color = Simian::Color(1.0f, 1.0f, 1.0f, 1.0f);
                break;
            case 'k':
                color = Simian::Color(0.0f, 0.0f, 0.0f, 1.0f);
                break;
			case 'o':
                color = Simian::Color(1.0f, 0.513f, 0.219f, 1.0f);
                break;
			case 'e':
                color = Simian::Color(0.588f, 0.545f, 0.545f, 1.0f);
                break;
			case 'p':
                color = Simian::Color(0.721f, 0.415f, 0.709f, 1.0f);
                break;
			case 'i':
                color = Simian::Color(1.0f, 0.525f, 1.0f, 1.0f);
                break;
			case 'l':
                color = Simian::Color(0.231f, 0.667f, 1.0f, 1.0f);
                break;
			case 'n':
                color = Simian::Color(0.755f, 0.405f, 0.119f, 1.0f);
                break;
            case 'y':
                color = Simian::Color(0.9f, 0.9f, 0.1f, 1.0f);
                break;
            }

            bitmapText_->AddColor(pos - acc, color);
            acc += 2;
            pos += 2;
            prevPos = pos;
        }

        // just add the left overs
        final += text_.substr(prevPos);
        bitmapText_->Text(final);
    }

    void CBitmapText::update_( Simian::DelegateParameter& )
    {
        if (alignment_.X() != bitmapText_->Alignment().X() ||
            alignment_.Y() != bitmapText_->Alignment().Y())
            bitmapText_->Alignment(alignment_);
    }

    void CBitmapText::render_( Simian::DelegateParameter& )
    {
        Simian::Matrix billboard = Simian::Matrix::Identity;

        if (layer_ == Simian::RenderQueue::RL_SCENE)
        {
            CCamera* cam;
            CCameraBase::SceneCamera(cam);

            // create a billboard matrix with the position of the world matrix
            /*
                | cz      -cxcy       cx |
                | 0       cz2 + cx2   cy |
                | -cx     -cycz       cz |
            */
            Simian::Vector3 z(cam->Camera().Direction());
            Simian::Vector3 x(Simian::Vector3::UnitY.Cross(z));
            x.Normalize();
            Simian::Vector3 y(z.Cross(x));

            billboard = Simian::Matrix(
                x.X(), y.X(), z.X(), transform_->World().Position().X(),
                x.Y(), y.Y(), z.Y(), transform_->World().Position().Y(),
                x.Z(), y.Z(), z.Z(), transform_->World().Position().Z(),
                0.0f, 0.0f, 0.0f, 1.0f);
        }
        else
            billboard = transform_->World();

        renderQueue_->AddRenderObject(RenderObject(*fontMaterial_, layer_, 
            billboard * Simian::Matrix::Scale(scale_.X(), scale_.Y(), 1.0f), 
            Simian::Delegate::CreateDelegate<CBitmapText, &CBitmapText::draw_>(this),
            0));
    }

    void CBitmapText::draw_( Simian::DelegateParameter& )
    {
        bitmapText_->Draw(0);
    }

    //--------------------------------------------------------------------------

    void CBitmapText::OnSharedLoad()
    {
        fontMaterial_ = materialCache_->Load(materialFile_);

        bitmapFont_ = new BitmapFont(graphicsDevice_);
        bitmapFont_->LoadFile(fontFile_);

        RegisterCallback(P_UPDATE, Simian::Delegate::CreateDelegate<CBitmapText, &CBitmapText::update_>(this));
        RegisterCallback(P_RENDER, Simian::Delegate::CreateDelegate<CBitmapText, &CBitmapText::render_>(this));
    }

    void CBitmapText::OnSharedUnload()
    {
        if (bitmapFont_)
            bitmapFont_->Unload();
        delete bitmapFont_;
        bitmapFont_ = 0;
    }

    void CBitmapText::OnAwake()
    {
        ComponentByType(C_TRANSFORM, transform_);

        bitmapFont_->CreateBitmapText(bitmapText_, cacheSize_);
        updateText_();

        // update opacity
        bitmapText_->Opacity(opacity_);
    }

    void CBitmapText::OnDeinit()
    {
        bitmapFont_->DestroyBitmapText(bitmapText_);
        bitmapText_ = 0;
    }

    void CBitmapText::Serialize( FileObjectSubNode& data )
    {
        data.AddData("MaterialLocation", materialFile_);
        data.AddData("FontLocation", fontFile_);
        data.AddData("Text", text_);
        data.AddData("CacheSize", cacheSize_);
        data.AddData("Opacity", opacity_);

        data.AddData("Layer", layer_ == Simian::RenderQueue::RL_PRESCENE ? "PRESCENE" : 
                              layer_ == Simian::RenderQueue::RL_SCENE ? "SCENE" : 
                              layer_ == Simian::RenderQueue::RL_POSTSCENE ? "POSTSCENE" : 
                              layer_ == Simian::RenderQueue::RL_PREUI ? "PREUI" : 
                              layer_ == Simian::RenderQueue::RL_UI ? "UI" : 
                              layer_ == Simian::RenderQueue::RL_POSTUI ? "POSTUI" : 
                              "SCENE");

        FileObjectSubNode* alignment = data.AddObject("Alignment");
        alignment->AddData("X", alignment_.X());
        alignment->AddData("Y", alignment_.Y());

        FileObjectSubNode* scale = data.AddObject("Scale");
        scale->AddData("X", scale_.X());
        scale->AddData("Y", scale_.Y());
    }

    void CBitmapText::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("MaterialLocation", materialFile_, materialFile_);
        data.Data("FontLocation", fontFile_, fontFile_);
        data.Data("Text", text_, text_);
        data.Data("CacheSize", cacheSize_, cacheSize_);
        data.Data("Opacity", opacity_, opacity_);

        std::string layer = "";
        data.Data("Layer", layer, layer);
        layer_ = static_cast<u8>(layer == "PRESCENE" ? Simian::RenderQueue::RL_PRESCENE :
                 layer == "SCENE" ? Simian::RenderQueue::RL_SCENE :
                 layer == "POSTSCENE" ? Simian::RenderQueue::RL_POSTSCENE :
                 layer == "PREUI" ? Simian::RenderQueue::RL_PREUI :
                 layer == "UI" ? Simian::RenderQueue::RL_UI :
                 layer == "POSTUI" ? Simian::RenderQueue::RL_POSTUI :
                 layer_);

        const FileObjectSubNode* alignment = data.Object("Alignment");
        if (alignment)
        {
            float x, y;
            alignment->Data("X", x, alignment_.X());
            alignment->Data("Y", y, alignment_.Y());
            alignment_ = Vector2(x, y);
        }

        const FileObjectSubNode* scale = data.Object("Scale");
        if (scale)
        {
            float x, y;
            scale->Data("X", x, scale_.X());
            scale->Data("Y", y, scale_.Y());
            scale_ = Vector2(x, y);
        }

        if (bitmapText_)
        {
            updateText_();
            Opacity(opacity_);
        }
    }
}
