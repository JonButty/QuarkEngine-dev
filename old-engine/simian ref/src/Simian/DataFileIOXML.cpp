/*************************************************************************/
/*!
\file		DataFileIOXML.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/SimianPlatform.h"
#include "Simian/DataFileIOXML.h"
#include "Simian/FileObjectSubNode.h"
#include "Simian/FileArraySubNode.h"
#include "Simian/FileDataSubNode.h"
#include "Simian/Utility.h"

#include "tinyxml.h"

#include <string>
#include <fstream>

namespace Simian
{
    bool DataFileIOXML::Read( const DataLocation& data )
    {
        if(!data)return false;

        u8 flags = Flags();
        if (SIsFlagSet(flags, SF_OPENED))
            Close();

        TiXmlDocument doc;
        doc.Parse(std::string(data.Memory(), data.Memory() + data.Size()).c_str());

        if(doc.Error())
            return false;

        TiXmlHandle hDoc(&doc);
        TiXmlElement* rootNode = hDoc.FirstChildElement().ToElement();

        if (!rootNode)
            return false;

        FileObjectSubNode* root = registerNodeList_(rootNode);
        root->Name(rootNode->Value());

        Root(root);

        delete root;
        root = 0;

        SFlagSet(flags, SF_OPENED);
        Flags(flags);

        return true;
    }

    FileObjectSubNode* DataFileIOXML::registerNodeList_(TiXmlElement* const pRoot)
    {
        if (!pRoot) return NULL;

        TiXmlElement* pNode = pRoot->FirstChildElement();
        FileObjectSubNode* root = new FileObjectSubNode;
        root->Name(pNode->Value());

        while (pNode)
        {
            root->AddChild(deserialize_(pNode));

            pNode = pNode->NextSiblingElement();
        }
        return root;
    }

    FileSubNode* DataFileIOXML::deserialize_ ( TiXmlElement* const pNode)
    {
        if (!pNode)return NULL;

        TiXmlElement* parent = pNode;

        const s8* type = pNode->Attribute("type");

        // Data node
        const s8* text = parent->GetText();
        if (!type && (text || parent->NoChildren()))
        {
            FileDataSubNode* node = new FileDataSubNode(parent->NoChildren() ? "" : text);
            node->Name(parent->Value());
            return node;
        }

        // Get nodes under the root
        TiXmlElement* pChild = parent->FirstChildElement();

        // Array node
        if(type && std::string(type) == "array")
        {
            FileArraySubNode* node = new FileArraySubNode;
            // First element of the array
            node->Name(parent->Value());
            while(pChild)
            {
                node->AddChild(deserialize_(pChild));
                pChild = pChild->NextSiblingElement();
            }
            return node;
        }
              
        // Object node
        FileObjectSubNode* node = new FileObjectSubNode;
        node->Name(parent->Value());

        // Child
        while(pChild)
        {
            node->AddChild(deserialize_(pChild));
            pChild = pChild->NextSiblingElement();
        }
        return node;
    }

    void DataFileIOXML::Write( const std::string& path )
    {
        /*TiXmlDocument doc;
        
        //Attempt to open file
        FILE * file = NULL;
        file = fopen(path.c_str(), "w");

        if (!file)
            return;

        // Link root to doc
        TiXmlElement * root = new TiXmlElement(Root()->Name().c_str());
        doc.LinkEndChild(root);
        FileObjectSubNode::Iterator i = Root()->Begin();

        while(i != Root()->End())
        {
            serialize_(root, *i);
            ++i;
        }
        doc.SaveFile(file);
        fclose(file);*/

        std::ofstream file(path.c_str());
        file << AsString();
    }

    void DataFileIOXML::serialize_ (TiXmlElement * linkTo, FileSubNode* linkFrom)
    {
        if(!(linkTo && linkFrom)) return;
        if(linkFrom->Type() == FileSubNode::FNT_OBJECT)
        {
            // All nodes in the loop will link to the parent
            TiXmlElement* parent = new TiXmlElement(linkFrom->Name().c_str());

            FileObjectSubNode* objectNode = reinterpret_cast<FileObjectSubNode*>(linkFrom);
            // Iterator to the first child of the current node
            FileObjectSubNode::Iterator i = objectNode->Begin();
            
            while(i != objectNode->End())
            {
                serialize_(parent,*i);
                ++i;
            }
            linkTo->LinkEndChild(parent);
        }
        else if(linkFrom->Type() == FileSubNode::FNT_ARRAY)
        {
            // All nodes in the loop will link to the parent
            TiXmlElement* parent = new TiXmlElement(linkFrom->Name().c_str());

            FileArraySubNode* arrayNode = reinterpret_cast<FileArraySubNode*>(linkFrom);

            if (!arrayNode) return;
            for (size_t i = 0; i < arrayNode->Size(); ++i)
            {
                serialize_(parent,arrayNode->Object(i));
            }
            parent->SetAttribute("type","array");
            linkTo->LinkEndChild(parent);
        }
        else if(linkFrom->Type() == FileSubNode::FNT_DATA)
        {
           FileDataSubNode* dataNode = reinterpret_cast<FileDataSubNode*>(linkFrom);
           TiXmlElement* node = new TiXmlElement(dataNode->Name().c_str());
           TiXmlText * text = new TiXmlText(dataNode->AsString().c_str());
           node->LinkEndChild(text);
           linkTo->LinkEndChild(node);
        }
    }

    std::string DataFileIOXML::AsString()
    {
        TiXmlDocument doc;

        // Link root to doc
        TiXmlElement * root = new TiXmlElement(Root()->Name().c_str());
        doc.LinkEndChild(root);
        FileObjectSubNode::Iterator i = Root()->Begin();

        while(i != Root()->End())
        {
            serialize_(root, *i);
            ++i;
        }

        // create a printer and set the indentation
        TiXmlPrinter printer;
        printer.SetIndent("  ");

        // return the doc as a string
        doc.Accept(&printer);

        // return string with printer's contents
        return std::string(printer.CStr());
    }
}
