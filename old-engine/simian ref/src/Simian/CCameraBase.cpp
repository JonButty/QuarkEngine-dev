/************************************************************************/
/*!
\file		CCameraBase.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CCamera.h"
#include "Simian/Camera.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/RenderQueue.h"
#include "Simian/RenderTexture.h"
#include "Simian/EnginePhases.h"
#include "Simian/CompositorChain.h"
#include "Simian/Game.h"
#include "Simian/Compositor.h"

namespace Simian
{
    GraphicsDevice* CCameraBase::device_ = 0;
    Simian::RenderQueue* CCameraBase::renderQueue_ = 0;
    CCameraBase* CCameraBase::sceneCamera_ = 0;

    CCameraBase::CCameraBase()
        : camera_(new Simian::Camera(device_)),
          renderTexture_(0),
          clearColor_(0.0f, 0.4f, 0.0f, 1.0f),
          up_(Vector3::UnitY),
          layers_(0),
          uiCamera_(false),
          composite_(true)
    {
        device_->CreateRenderTexture(*renderTexture_);
        camera_->RenderTexture(*renderTexture_);
    }

    CCameraBase::~CCameraBase()
    {
        device_->DestroyRenderTexture(*renderTexture_);
        delete *camera_;
    }

    //--------------------------------------------------------------------------

    const Simian::Color& CCameraBase::ClearColor() const
    {
        return clearColor_;
    }

    void CCameraBase::ClearColor( const Simian::Color& val )
    {
        clearColor_ = val;
        camera_->ClearColor(val);
    }

    const Simian::Vector3& CCameraBase::Up() const
    {
        return up_;
    }

    void CCameraBase::Up( const Simian::Vector3& val )
    {
        up_ = val;
    }

    GraphicsDevice* CCameraBase::Device()
    {
        return device_;
    }

    void CCameraBase::Device( GraphicsDevice* val )
    {
        device_ = val;
    }

    Simian::RenderQueue* CCameraBase::RenderQueue()
    {
        return renderQueue_;
    }

    void CCameraBase::RenderQueue( Simian::RenderQueue* val )
    {
        renderQueue_ = val;
    }

    Simian::Camera& CCameraBase::Camera() const
    {
        return *camera_;
    }

    //--------------------------------------------------------------------------

    void CCameraBase::OnAwake()
    {
        if (renderTextureName_ != "")
            camera_->RenderTextureName(renderTextureName_);
        camera_->ClearColor(clearColor_);
        camera_->Layers(layers_);

        ComponentByType(C_TRANSFORM, transform_);

        renderTexture_->LoadImmediate(device_->Size(), Texture::SF_RGBA);
        renderTexture_->AutoResize(composite_);
        renderQueue_->AddCamera(camera_);

        if (SIsFlagSet(layers_, RenderQueue::RL_SCENE))
            sceneCamera_ = this;

        if (compositors_.size())
        {
            // create compositor chain
            CompositorChain* chain = new CompositorChain(device_, Game::Instance().FullscreenQuad());
            
            for (Simian::u32 i = 0; i < compositors_.size(); ++i)
            {
                Compositor* compositor = new Compositor(device_, &Game::Instance().MaterialCache());
                if (compositor->Load(compositors_[i]))
                    chain->AddCompositor(compositor);
            }

            camera_->CompositorChain(chain);
        }
    }

    void CCameraBase::OnDeinit()
    {
        // destroy compositors and compositor chain
        if (camera_->CompositorChain())
        {
            for (std::vector<Compositor*>::const_iterator i = 
                 camera_->CompositorChain()->Compositors().begin();
                 i != camera_->CompositorChain()->Compositors().end(); ++i)
            {
                 (*i)->Unload();
                 delete (*i);
            }
            delete camera_->CompositorChain();
        }

        renderQueue_->RemoveCamera(camera_);
        renderTexture_->Unload();
    }

    void CCameraBase::SetAsSceneCamera()
    {
        sceneCamera_ = this;
    }

    void CCameraBase::Serialize( FileObjectSubNode& data )
    {
        data.AddData("RenderTextureName", renderTextureName_);

        FileObjectSubNode* clearColor = data.AddObject("ClearColor");
        clearColor->AddData("R", clearColor_.R());
        clearColor->AddData("G", clearColor_.G());
        clearColor->AddData("B", clearColor_.B());
        clearColor->AddData("A", clearColor_.A());

        FileObjectSubNode* up = data.AddObject("Up");
        up->AddData("X", up_.X());
        up->AddData("Y", up_.Y());
        up->AddData("Z", up_.Z());

        FileArraySubNode* layers = data.AddArray("Layers");
        if (SIsFlagSet(layers_, Simian::RenderQueue::RL_POSTSCENE))
            layers->AddData("Layer", "POSTSCENE");
        if (SIsFlagSet(layers_, Simian::RenderQueue::RL_POSTUI))
            layers->AddData("Layer", "POSTUI");
        if (SIsFlagSet(layers_, Simian::RenderQueue::RL_PRESCENE))
            layers->AddData("Layer", "PRESCENE");
        if (SIsFlagSet(layers_, Simian::RenderQueue::RL_PREUI))
            layers->AddData("Layer", "PREUI");
        if (SIsFlagSet(layers_, Simian::RenderQueue::RL_SCENE))
            layers->AddData("Layer", "SCENE");
        if (SIsFlagSet(layers_, Simian::RenderQueue::RL_UI))
            layers->AddData("Layer", "UI");

        data.AddData("UICamera", uiCamera_);
        data.AddData("Composite", composite_);

        if (compositors_.size())
        {
            FileArraySubNode* compositors = data.AddArray("Compositors");
            for (Simian::u32 i = 0; i < compositors_.size(); ++i)
                compositors->AddData("Compositor", compositors_[i]);
        }
    }

    void CCameraBase::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("RenderTextureName", renderTextureName_, renderTextureName_);

        const FileObjectSubNode* clearColor = data.Object("ClearColor");
        if (clearColor)
        {
            const FileDataSubNode* R, *G, *B, *A;
            R = clearColor->Data("R");
            G = clearColor->Data("G");
            B = clearColor->Data("B");
            A = clearColor->Data("A");
            clearColor_ = Simian::Color(
                R ? R->AsF32() : clearColor_.R(),
                G ? G->AsF32() : clearColor_.G(),
                B ? B->AsF32() : clearColor_.B(),
                A ? A->AsF32() : clearColor_.A());
        }

        const FileObjectSubNode* up = data.Object("Up");
        if (up)
        {
            const FileDataSubNode* X, *Y, *Z;
            X = up->Data("X");
            Y = up->Data("Y");
            Z = up->Data("Z");
            up_ = Simian::Vector3(
                X ? X->AsF32() : up_.X(),
                Y ? Y->AsF32() : up_.Y(),
                Z ? Z->AsF32() : up_.Z());
        }

        const FileArraySubNode* layers = data.Array("Layers");
        if (layers)
        {
            for (u32 i = 0; i < layers->Size(); ++i)
            {
                const FileDataSubNode* data = layers->Data(i);
                
                if (data->AsString() == "PRESCENE")
                    SFlagSet(layers_, Simian::RenderQueue::RL_PRESCENE);
                else if (data->AsString() == "SCENE")
                {
                    sceneCamera_ = this;
                    SFlagSet(layers_, Simian::RenderQueue::RL_SCENE);
                }
                else if (data->AsString() == "POSTSCENE")
                    SFlagSet(layers_, Simian::RenderQueue::RL_POSTSCENE);
                else if (data->AsString() == "PREUI")
                    SFlagSet(layers_, Simian::RenderQueue::RL_PREUI);
                else if (data->AsString() == "UI")
                    SFlagSet(layers_, Simian::RenderQueue::RL_UI);
                else if (data->AsString() == "POSTUI")
                    SFlagSet(layers_, Simian::RenderQueue::RL_POSTUI);
            }
        }

        data.Data("UICamera", uiCamera_, uiCamera_);
        data.Data("Composite", composite_, composite_);

        const FileArraySubNode* compositors = data.Array("Compositors");
        if (compositors)
        {
            compositors_.clear();
            for (Simian::u32 i = 0; i < compositors->Size(); ++i)
            {
                const FileDataSubNode* compositor = compositors->Data(i);
                compositors_.push_back(compositor->AsString());
            }
        }
    }
}
