/*************************************************************************/
/*!
\file		FileArraySubNode.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/FileArraySubNode.h"
#include "Simian/FileDataSubNode.h"
#include "Simian/FileObjectSubNode.h"

namespace Simian
{
    FileArraySubNode::FileArraySubNode()
        : FileSubNode(FNT_ARRAY)
    {
    }

    FileArraySubNode::FileArraySubNode( const FileArraySubNode& copy )
        : FileSubNode(copy)
    {
        for (u32 i = 0; i < copy.Size(); ++i)
        {
            FileSubNode* child = 0;
            if (copy[i]->Type() == FNT_ARRAY)
                child = new FileArraySubNode(*copy.Array(i));
            else if (copy[i]->Type() == FNT_DATA)
                child = new FileDataSubNode(*copy.Data(i));
            else if (copy[i]->Type() == FNT_OBJECT)
                child = new FileObjectSubNode(*copy.Object(i));
            if (child)
                AddChild(child);
        }
    }

    FileArraySubNode::~FileArraySubNode()
    {
        for (u32 i = 0; i < children_.size(); ++i)
            delete children_[i];
        children_.clear();
    }

    //--------------------------------------------------------------------------

    Simian::u32 FileArraySubNode::Size() const
    {
        return children_.size();
    }

    //--------------------------------------------------------------------------

    FileSubNode* FileArraySubNode::operator[]( u32 index )
    {
        return children_[index];
    }

    const FileSubNode* FileArraySubNode::operator[]( u32 index ) const
    {
        return children_[index];
    }

    FileArraySubNode& FileArraySubNode::operator=( const FileArraySubNode& copy )
    {
        if (this == &copy)
            return *this;

        FileSubNode::operator=(copy);

        for (u32 i = 0; i < children_.size(); ++i)
            delete children_[i];
        children_.clear();

        for (u32 i = 0; i < copy.Size(); ++i)
        {
            FileSubNode* child = 0;
            if (copy[i]->Type() == FNT_ARRAY)
                child = new FileArraySubNode(*copy.Array(i));
            else if (copy[i]->Type() == FNT_DATA)
                child = new FileDataSubNode(*copy.Data(i));
            else if (copy[i]->Type() == FNT_OBJECT)
                child = new FileObjectSubNode(*copy.Object(i));
            if (child)
                AddChild(child);
        }

        return *this;
    }

    bool FileArraySubNode::operator== ( const FileSubNode& o )
    {
        const FileArraySubNode& other = static_cast<const FileArraySubNode&>(o);

        // instant rejection
        if (children_.size() != other.Size())
            return false;

        // loop through and make sure all children are the same
        for (u32 i = 0; i < children_.size(); ++i)
        {
            if (children_[i]->Type() != other.children_[i]->Type())
                return false;

            if (*children_[i] == *other.children_[i]);
            else return false;
        }
        
        return true;
    }

    //--------------------------------------------------------------------------

    void FileArraySubNode::AddChild( FileSubNode* node )
    {
        children_.push_back(node);
    }

    FileObjectSubNode* FileArraySubNode::Object( u32 name )
    {
        return Get<FileObjectSubNode>(name);
    }

    const FileObjectSubNode* FileArraySubNode::Object( u32 name ) const
    {
        return Get<FileObjectSubNode>(name);
    }

    FileArraySubNode* FileArraySubNode::Array( u32 name )
    {
        return Get<FileArraySubNode>(name);
    }

    const FileArraySubNode* FileArraySubNode::Array( u32 name ) const
    {
        return Get<FileArraySubNode>(name);
    }

    FileDataSubNode* FileArraySubNode::Data( u32 name )
    {
        return Get<FileDataSubNode>(name);
    }

    const FileDataSubNode* FileArraySubNode::Data( u32 name ) const
    {
        return Get<FileDataSubNode>(name);
    }

    FileObjectSubNode* FileArraySubNode::AddObject( const std::string& name )
    {
        FileObjectSubNode* object = new FileObjectSubNode();
        object->Name(name);
        AddChild(object);
        return object;
    }

    FileArraySubNode* FileArraySubNode::AddArray( const std::string& name )
    {
        FileArraySubNode* object = new FileArraySubNode();
        object->Name(name);
        AddChild(object);
        return object;
    }
}
