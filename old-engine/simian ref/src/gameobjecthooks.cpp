/*************************************************************************/
/*!
\file		gameobjecthooks.cpp
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
#include "Simian/Entity.h"
#include "Simian/LogManager.h"

#include "GCEditor.h"
#include "StateEditor.h"
#include "StateGame.h"
#include "ComponentTypes.h"

#define DESCENDED_EXPORT __declspec(dllexport) __stdcall

std::string temp;

extern "C"
{
    __declspec(dllexport) const char* __stdcall GetGameObjectName(Simian::Entity* node)
    {
        return node->Name().c_str();
    }

    __declspec(dllexport) Simian::u32 __stdcall GetComponentCount()
    {
        return Descension::GC_TOTAL;
    }

    __declspec(dllexport) Simian::EntityComponent* __stdcall GetComponent(Simian::Entity* entity, int id)
    {
        Simian::EntityComponent* component;
        entity->ComponentByType(id, component);
        return component;
    }

    __declspec(dllexport) const char* __stdcall SerializeComponent(Simian::EntityComponent* component, int id)
    {
        Simian::FileObjectSubNode componentNode;
        componentNode.Name("Component");
        std::string type = Simian::GameFactory::Instance().ComponentTable().Key(id);
        componentNode.AddData("Type", type);
        component->Serialize(componentNode);
        Simian::DataFileIODefault dataFile;
        dataFile.Root(&componentNode);
        // we need to store this externally, stack var is cleared.. lol
        temp = dataFile.AsString(); 
        return temp.c_str();
    }

    void DESCENDED_EXPORT DeserializeComponent(Simian::EntityComponent* component, const Simian::s8* data)
    {
        Simian::DataFileIODefault dataDoc;
        dataDoc.Read(Simian::DataLocation(const_cast<Simian::s8*>(data), std::strlen(data)));
        component->Deserialize(*dataDoc.Root());
    }

    void DESCENDED_EXPORT SpawnGameObject(const char* objectId)
    {
        Descension::GCEditor::Instance().SpawnObject(objectId);
    }

    void DESCENDED_EXPORT DeleteSelectedGameObjects()
    {
        Descension::GCEditor::Instance().DeleteSelectedObjects();
    }

    void DESCENDED_EXPORT DeleteGameObject(Simian::Entity* gameObject)
    {
        Descension::GCEditor::Instance().DeleteObject(gameObject);
    }

    void DESCENDED_EXPORT ReparentGameObject(Simian::Entity* entity, Simian::Entity* newParent)
    {
        entity->Reparent(newParent);
    }

    void DESCENDED_EXPORT RenameGameObject(Simian::Entity* entity, const char* newName)
    {
        entity->Name(newName);
    }
};

#endif
