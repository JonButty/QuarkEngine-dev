/************************************************************************/
/*!
\file		CModelAttachment.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CModelAttachment.h"
#include "Simian/EngineComponents.h"
#include "Simian/CModel.h"
#include "Simian/LogManager.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CModelAttachment> CModelAttachment::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_MODELATTACHMENT);

    CModelAttachment::CModelAttachment()
        : model_(0)
    {
    }

    //--------------------------------------------------------------------------

    void CModelAttachment::OnInit()
    {
        ComponentByType(C_MODEL, model_);

        if (!model_)
        {
            SWarn("Entity: " << ParentEntity()->Name() << " has no model component for model attachment.");
            return;
        }

        for (AttachmentTable::iterator i = attachments_.begin(); i != attachments_.end(); ++i)
        {
            // find a child..
            Simian::Entity* entity = ParentEntity()->ChildByName(i->second);
            if (!entity)
            {
                SWarn("Child entity: " << i->second << " does not exist for model attachment.");
                continue;
            }
            model_->AttachEntityToJoint(i->first, entity);
        }
    }

    void CModelAttachment::Serialize( FileObjectSubNode& data )
    {
        if (attachments_.size())
        {
            FileArraySubNode* attachments = data.AddArray("Attachments");
            for (AttachmentTable::iterator i = attachments_.begin(); i != attachments_.end(); ++i)
            {
                FileObjectSubNode* attachment = attachments->AddObject("Attachment");
                attachment->AddData("Bone", i->first);
                attachment->AddData("Entity", i->second);
            }
        }
    }

    void CModelAttachment::Deserialize( const FileObjectSubNode& data )
    {
        const FileArraySubNode* attachments = data.Array("Attachments");
        if (attachments)
        {
            for (u32 i = 0; i < attachments->Size(); ++i)
            {
                const FileObjectSubNode* attachment = attachments->Object(i);
                std::string bone, entity;
                attachment->Data("Bone", bone, "");
                attachment->Data("Entity", entity, "");
                if (bone == "" || entity == "")
                    continue;
                attachments_.push_back(std::make_pair(bone, entity));
            }
        }
    }
}
