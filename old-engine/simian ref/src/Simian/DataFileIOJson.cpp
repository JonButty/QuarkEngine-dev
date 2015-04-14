/*************************************************************************/
/*!
\file		DataFileIOJson.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/DataFileIOJson.h"
#include "Simian/DataLocation.h"
#include "Simian/Utility.h"
#include "Simian/FileObjectSubNode.h"
#include "Simian/FileArraySubNode.h"
#include "Simian/FileDataSubNode.h"

#include "json/json.h"

#include <fstream>

namespace Simian
{
    FileSubNode* DataFileIOJson::createNode_( Json::Value& value, const std::string& name )
    {
        // check the basic types
        if (value.isObject())
        {
            FileObjectSubNode* node = new FileObjectSubNode();
            node->Name(name);

            // loop through children
            Json::Value::Members members = value.getMemberNames();
            for (u32 i = 0; i < members.size(); ++i)
            {
                FileSubNode* child = createNode_(value[members[i]], members[i]);
                node->AddChild(child);
            }

            return node;
        }
        else if (value.isArray())
        {
            FileArraySubNode* node = new FileArraySubNode();
            node->Name(name);

            // loop through children
            for (u32 i = 0; i < value.size(); ++i)
            {
                FileSubNode* child = createNode_(value[i], "ArrayNode");
                node->AddChild(child);
            }

            return node;
        }
        else
        {
            FileDataSubNode* node;

            if (value.isBool())
                node = new FileDataSubNode(value.asBool());
            else if (value.isString())
                node = new FileDataSubNode(value.asString());
            else
                node = new FileDataSubNode(
                    static_cast<float>(value.asDouble()));

            node->Name(name);

            return node;
        }
    }

    static Json::Value getJsonEquivalent_(FileSubNode* node)
    {
        if (node->Type() == FileSubNode::FNT_ARRAY)
            return Json::Value(Json::arrayValue);
        else if (node->Type() == FileSubNode::FNT_DATA)
        {
            FileDataSubNode* dataNode = reinterpret_cast<FileDataSubNode*>(node);
            if (dataNode->IsBool())
                return Json::Value(dataNode->AsBool());
            else if (dataNode->IsFloat())
                return Json::Value(dataNode->AsF32());
            else if (dataNode->IsString())
                return Json::Value(dataNode->AsString());
        }

        return Json::Value(Json::objectValue);
    }

    void DataFileIOJson::createJsonNode_( Json::Value& value, FileSubNode* node )
    {
        if (node->Type() == FileSubNode::FNT_ARRAY)
        {
            FileArraySubNode* arrayNode = reinterpret_cast<FileArraySubNode*>(node);
            for (u32 i = 0; i < arrayNode->Size(); ++i)
            {
                FileSubNode* child = arrayNode->Get<FileSubNode>(i);
                Json::Value childVal(getJsonEquivalent_(child));
                createJsonNode_(childVal, child);
                value.append(childVal);
            }
        }
        else if (node->Type() == FileSubNode::FNT_OBJECT)
        {
            FileObjectSubNode* objectNode = reinterpret_cast<FileObjectSubNode*>(node);
            for (FileObjectSubNode::Iterator i = objectNode->Begin(); i != objectNode->End(); ++i)
            {
                Json::Value childVal(getJsonEquivalent_(*i));
                createJsonNode_(childVal, *i);
                value[(*i)->Name()] = childVal;
            }
        }
    }

    //--------------------------------------------------------------------------

    bool DataFileIOJson::Read( const DataLocation& data )
    {
        if (!data)
            return false;

        u8 flags = Flags();
        if (SIsFlagSet(flags, SF_OPENED))
            Close();

        Json::Value root;
        bool success = Json::Reader().parse(data.Memory(), data.Memory() + data.Size(), root);

        if (!success)
            return false;

        FileSubNode* fileroot = createNode_(root, "Root");
        Root(reinterpret_cast<FileObjectSubNode*>(fileroot));
        delete fileroot;

        SFlagSet(flags, SF_OPENED);
        Flags(flags);
        return true;
    }

    void DataFileIOJson::Write( const std::string& path )
    {
        Json::Value root(Json::objectValue);
        createJsonNode_(root, Root());

        std::ofstream file;
        file.open(path);

        if (!file)
            return;

        Json::StyledStreamWriter("    ").write(file, root);
    }

    std::string DataFileIOJson::AsString()
    {
        Json::Value root(Json::objectValue);
        createJsonNode_(root, Root());
        return root.toStyledString();
    }
}
