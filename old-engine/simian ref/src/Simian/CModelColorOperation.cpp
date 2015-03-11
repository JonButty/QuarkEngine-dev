/************************************************************************/
/*!
\file		CModelColorOperation.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CModelColorOperation.h"
#include "Simian/EngineComponents.h"
#include "Simian/CModel.h"
#include "Simian/Model.h"
#include "Simian/Material.h"
#include "Simian/Shader.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CModelColorOperation> CModelColorOperation::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_MODELCOLOROPERATION);

    CModelColorOperation::CModelColorOperation()
        : color_(1.0f, 1.0f, 1.0f, 1.0f),
          operation_(CO_MODULATE),
          model_(0)
    {
    }

    //--------------------------------------------------------------------------

    const Simian::Color& CModelColorOperation::Color() const
    {
        return color_;
    }

    void CModelColorOperation::Color( const Simian::Color& val )
    {
        color_ = val;
    }

    Simian::CModelColorOperation::ColorOperation CModelColorOperation::Operation() const
    {
        return operation_;
    }

    void CModelColorOperation::Operation( ColorOperation val )
    {
        operation_ = val;
    }
    
    //--------------------------------------------------------------------------

    void CModelColorOperation::apply_( DelegateParameter& )
    {
        // for all passes
        Material::Pass* pass = model_->RenderQueue()->ActivePass();

        for (std::vector<Material::TextureStage>::const_iterator j = pass->TextureStages().begin();
            j != pass->TextureStages().end(); ++j)
        {
            Texture* texture = (*j).Texture();

            oldStates_.push_back(texture->TextureState(Texture::TSF_COLOROP));
            oldStates_.push_back(texture->TextureState(Texture::TSF_COLORARG1));
            oldStates_.push_back(texture->TextureState(Texture::TSF_COLORARG2));
            oldStates_.push_back(texture->TextureState(Texture::TSF_CONSTANT));

            texture->TextureState(Texture::TSF_COLOROP, operation_);
            texture->TextureState(Texture::TSF_COLORARG1, Texture::TSA_TEXTURE);
            texture->TextureState(Texture::TSF_COLORARG2, Texture::TSA_CONSTANT);
            texture->TextureState(Texture::TSF_CONSTANT, color_.ColorU32());
        }

        // update the shader if there is one
        if (pass->Shader())
            pass->Shader()->Update();
    }

    void CModelColorOperation::unapply_( DelegateParameter& )
    {
        Material::Pass* pass = model_->RenderQueue()->ActivePass();

        u32 counter = 0;
        for (std::vector<Material::TextureStage>::const_iterator j = pass->TextureStages().begin();
            j != pass->TextureStages().end(); ++j)
        {
            Texture* texture = (*j).Texture();

            texture->TextureState(Texture::TSF_COLOROP, oldStates_[counter++]);
            texture->TextureState(Texture::TSF_COLORARG1, oldStates_[counter++]);
            texture->TextureState(Texture::TSF_COLORARG2, oldStates_[counter++]);
            texture->TextureState(Texture::TSF_CONSTANT, oldStates_[counter++]);
        }
        oldStates_.clear();
    }

    //--------------------------------------------------------------------------

    void CModelColorOperation::OnAwake()
    {
        ComponentByType(C_MODEL, model_);
        if (model_)
        {
            model_->DrawCallback(Delegate::CreateDelegate<CModelColorOperation, &CModelColorOperation::apply_>(this));
            model_->EndDrawCallback(Delegate::CreateDelegate<CModelColorOperation, &CModelColorOperation::unapply_>(this));
        }
    }

    void CModelColorOperation::Deserialize( const FileObjectSubNode& data )
    {
        const FileDataSubNode* operationNode = data.Data("Operation");
        if (operationNode)
        {
            operation_ = 
                operationNode->AsString() == "Modulate" ? CO_MODULATE :
                operationNode->AsString() == "Modulate2x" ? CO_MODULATE2X :
                operationNode->AsString() == "Modulate4x" ? CO_MODULATE4X :
                operationNode->AsString() == "Add" ? CO_ADD :
                operation_;
        }

        const FileObjectSubNode* colorNode = data.Object("Color");
        if (colorNode)
        {
            const FileDataSubNode* R = colorNode->Data("R");
            const FileDataSubNode* G = colorNode->Data("G");
            const FileDataSubNode* B = colorNode->Data("B");
            const FileDataSubNode* A = colorNode->Data("A");

            color_ = Simian::Color(
                R ? R->AsF32() : color_.R(),
                G ? G->AsF32() : color_.G(),
                B ? B->AsF32() : color_.B(),
                A ? A->AsF32() : color_.A());
        }
    }

    void CModelColorOperation::Serialize( FileObjectSubNode& data )
    {
        data.AddData("Operation", 
            operation_ == CO_MODULATE ? "Modulate" :
            operation_ == CO_MODULATE2X ? "Modulate2x" :
            operation_ == CO_MODULATE4X ? "Modulate4x" :
            operation_ == CO_ADD ? "Add" :
            "Modulate");

        Simian::FileObjectSubNode* colorNode = data.AddObject("Color");
        colorNode->AddData("R", color_.R());
        colorNode->AddData("G", color_.G());
        colorNode->AddData("B", color_.B());
        colorNode->AddData("A", color_.A());
    }
}
