/************************************************************************/
/*!
\file		CModelAttachment.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CMODELATTACHMENT_H_
#define SIMIAN_CMODELATTACHMENT_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

#include <vector>
#include <utility>

namespace Simian
{
    class CModel;

    class CModelAttachment: public EntityComponent
    {
    private:
        typedef std::vector<std::pair<std::string, std::string> > AttachmentTable;
    private:
        AttachmentTable attachments_;
        CModel* model_;

        static FactoryPlant<EntityComponent, CModelAttachment> factoryPlant_;
    public:
        CModelAttachment();

        void OnInit();

        void Serialize( FileObjectSubNode& data );
        void Deserialize( const FileObjectSubNode& data );
    };
}

#endif
