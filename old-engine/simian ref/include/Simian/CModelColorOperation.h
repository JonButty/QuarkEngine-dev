/************************************************************************/
/*!
\file		CModelColorOperation.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CMODELCOLOROPERATOR_H_
#define SIMIAN_CMODELCOLOROPERATOR_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"
#include "Color.h"
#include "Texture.h"

#include <vector>
#include <utility>

namespace Simian
{
    class CModel;

    class SIMIAN_EXPORT CModelColorOperation: public EntityComponent
    {
    public:
        enum ColorOperation
        {
            CO_MODULATE     = Texture::TSO_MODULATE,
            CO_MODULATE2X   = Texture::TSO_MODULATE2X,
            CO_MODULATE4X   = Texture::TSO_MODULATE4X,
            CO_ADD          = Texture::TSO_ADD
        };
    private:
        Simian::Color color_;
        ColorOperation operation_;
        CModel* model_;
        std::vector<u32> oldStates_;

        static FactoryPlant<EntityComponent, CModelColorOperation> factoryPlant_;
    public:
        const Simian::Color& Color() const;
        void Color(const Simian::Color& val);
        
        Simian::CModelColorOperation::ColorOperation Operation() const;
        void Operation(Simian::CModelColorOperation::ColorOperation val);
    private:
        void apply_(DelegateParameter& param);
        void unapply_(DelegateParameter& param);
    public:
        CModelColorOperation();

        void OnAwake();

        void Deserialize(const FileObjectSubNode& data);
        void Serialize(FileObjectSubNode& data);
    };
}

#endif
