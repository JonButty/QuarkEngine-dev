/*************************************************************************/
/*!
\file		scenehooks.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifdef _DESCENDED

#include "Simian/SimianPlatform.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"

#include "GCEditor.h"
#include "StateEditor.h"
#include "StateGame.h"
#include "Simian/LogManager.h"

#define DESCENDED_EXPORT __declspec(dllexport) __stdcall

extern "C"
{
    Simian::u32 DESCENDED_EXPORT GetGameObjectCount(Simian::Entity* node)
    {
        if (node)
            return node->Children().size();
        else
        {
            Simian::Scene& scene = *Descension::GCEditor::Instance().ParentScene();
            return scene.RootEntities().size();
        }
    }

    __declspec(dllexport) Simian::Entity* __stdcall GetGameObject(Simian::Entity* node, Simian::u32 index)
    {
        if (node)
            return node->ChildByIndex(index);
        else
            return Descension::GCEditor::Instance().ParentScene()->EntityByIndex(index);
    }

    __declspec(dllexport) Simian::Entity* __stdcall GetSelectedGameObject()
    {
        return Descension::GCEditor::Instance().SelectedObject();
    }

    void DESCENDED_EXPORT CreateAnonymousGameObject(const char* entity, Simian::Entity* parent)
    {
        Descension::GCEditor::Instance().CreateAnonymousGameObject(entity, parent);
    }

    __declspec(dllexport) Simian::u32 __stdcall GetPoolFileCount()
    {
        return Descension::GCEditor::Instance().ParentScene()->PoolFiles().size();
    }

    void DESCENDED_EXPORT GetPoolFilePath(Simian::s32 i, char* path)
    {
        // copy to c_str to path.
        strcpy(path, Descension::GCEditor::Instance().ParentScene()->PoolFiles()[i].c_str());
    }

    void DESCENDED_EXPORT LinkPoolFile(const char* path)
    {
        Descension::GCEditor::Instance().ParentScene()->LinkPool(path);
    }

    void DESCENDED_EXPORT UnlinkPoolFile(const char* path)
    {
        Descension::GCEditor::Instance().ParentScene()->UnlinkPool(path);
    }
};

#endif
