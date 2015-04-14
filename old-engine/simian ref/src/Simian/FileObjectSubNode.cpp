/*************************************************************************/
/*!
\file		FileObjectSubNode.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/FileObjectSubNode.h"
#include "Simian/FileArraySubNode.h"
#include "Simian/FileDataSubNode.h"
#include "Simian/LogManager.h"

namespace Simian
{
    FileObjectSubNode::FileObjectSubNode()
        : FileSubNode(FNT_OBJECT)
    {
    }

    FileObjectSubNode::FileObjectSubNode( const FileObjectSubNode& copy )
        : FileSubNode(copy)
    {
        for (Iterator i = copy.Begin(); i != copy.End(); ++i)
        {
            FileSubNode* child = 0;
            if ((*i)->Type() == FNT_ARRAY)
                child = new FileArraySubNode(*reinterpret_cast<FileArraySubNode*>(*i));
            else if ((*i)->Type() == FNT_DATA)
                child = new FileDataSubNode(*reinterpret_cast<FileDataSubNode*>(*i));
            else if ((*i)->Type() == FNT_OBJECT)
                child = new FileObjectSubNode(*reinterpret_cast<FileObjectSubNode*>(*i));
            if (child)
                AddChild(child);
        }
    }

    FileObjectSubNode::~FileObjectSubNode()
    {
        for (Iterator i = children_.begin(); i != children_.end(); ++i)
            delete *i;
        children_.clear();
    }

    //--------------------------------------------------------------------------

    FileObjectSubNode::Iterator FileObjectSubNode::Begin()
    {
        return children_.begin();
    }

    FileObjectSubNode::Iterator FileObjectSubNode::End()
    {
        return children_.end();
    }

    FileObjectSubNode::ConstIterator FileObjectSubNode::Begin() const
    {
        return children_.begin();
    }

    FileObjectSubNode::ConstIterator FileObjectSubNode::End() const
    {
        return children_.end();
    }

    Simian::u32 FileObjectSubNode::Count() const
    {
        return children_.size();
    }

    //--------------------------------------------------------------------------

    FileSubNode* FileObjectSubNode::operator[]( const std::string& name )
    {
        Iterator i = std::find_if(children_.begin(), children_.end(), FileObjectFind(name));
        return i != children_.end() ? *i : NULL;
    }

    const FileSubNode* FileObjectSubNode::operator[]( const std::string& name ) const
    {
        ConstIterator i = std::find_if(children_.begin(), children_.end(), FileObjectFind(name));
        return i != children_.end() ? *i : NULL;
    }

    FileObjectSubNode& FileObjectSubNode::operator=( const FileObjectSubNode& copy )
    {
        if (this == &copy)
            return *this;

        FileSubNode::operator=(copy);

        for (Iterator i = children_.begin(); i != children_.end(); ++i)
            delete *i;
        children_.clear();

        for (Iterator i = copy.Begin(); i != copy.End(); ++i)
        {
            FileSubNode* child = 0;
            if ((*i)->Type() == FNT_ARRAY)
                child = new FileArraySubNode(*reinterpret_cast<FileArraySubNode*>(*i));
            else if ((*i)->Type() == FNT_DATA)
                child = new FileDataSubNode(*reinterpret_cast<FileDataSubNode*>(*i));
            else if ((*i)->Type() == FNT_OBJECT)
                child = new FileObjectSubNode(*reinterpret_cast<FileObjectSubNode*>(*i));
            if (child)
                AddChild(child);
        }

        return *this;
    }

    bool FileObjectSubNode::operator==( const FileSubNode& o )
    {
        const FileObjectSubNode& other = static_cast<const FileObjectSubNode&>(o);

        if (children_.size() != other.children_.size())
            return false;

        // compare all children
        for (Iterator i = children_.begin(), j = other.children_.begin();
             i != children_.end(), j != other.children_.end(); ++i, ++j)
        {
            // instant rejection
            if ((*i)->Type() != (*j)->Type())
                return false;

            // compare the values of the iterators
            if (**i == **j);
            else return false;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    void FileObjectSubNode::AddChild( FileSubNode* node )
    {
        Iterator i = std::find_if(children_.begin(), children_.end(), FileObjectFind(node->Name()));
        if (i != children_.end())
        {
            // discard child
            delete *i;
            children_.erase(i);
        }
        children_.insert(node);
    }

    void FileObjectSubNode::RemoveChild( FileSubNode* node )
    {
        Iterator i = children_.find(node);
        if (i != children_.end())
            children_.erase(i);
    }

    void FileObjectSubNode::RemoveChild( const std::string& name )
    {
        Iterator i = std::find_if(children_.begin(), children_.end(), FileObjectFind(name));
        if (i != children_.end())
            children_.erase(i);
    }

    FileObjectSubNode* FileObjectSubNode::Object( const std::string& name )
    {
        return Get<FileObjectSubNode>(name);
    }

    const FileObjectSubNode* FileObjectSubNode::Object( const std::string& name ) const
    {
        return Get<FileObjectSubNode>(name);
    }

    FileArraySubNode* FileObjectSubNode::Array( const std::string& name )
    {
        return Get<FileArraySubNode>(name);
    }

    const FileArraySubNode* FileObjectSubNode::Array( const std::string& name ) const
    {
        return Get<FileArraySubNode>(name);
    }

    FileDataSubNode* FileObjectSubNode::Data( const std::string& name )
    {
        return Get<FileDataSubNode>(name);
    }

    const FileDataSubNode* FileObjectSubNode::Data( const std::string& name ) const
    {
        return Get<FileDataSubNode>(name);
    }

    void FileObjectSubNode::Data( const std::string& name, bool& out, bool default) const
    {
        const FileDataSubNode* data = Data(name);
        out = data ? data->AsBool() : default;
    }

    void FileObjectSubNode::Data( const std::string& name, float& out, float default) const
    {
        const FileDataSubNode* data = Data(name);
        out = data ? data->AsF32() : default;
    }

    void FileObjectSubNode::Data( const std::string& name, std::string& out, std::string default) const
    {
        const FileDataSubNode* data = Data(name);
        out = data ? data->AsString() : default;
    }

    FileObjectSubNode* FileObjectSubNode::AddObject( const std::string& name )
    {
        FileObjectSubNode* object = new FileObjectSubNode();
        object->Name(name);
        AddChild(object);
        return object;
    }

    FileArraySubNode* FileObjectSubNode::AddArray( const std::string& name )
    {
        FileArraySubNode* object = new FileArraySubNode();
        object->Name(name);
        AddChild(object);
        return object;
    }

    bool FileObjectSubNode::Diff( const FileObjectSubNode& other )
    {
        bool same = true;

        Iterator i = children_.begin();
        while (i != children_.end())
        {
            const FileSubNode* othersChild = other.Get<FileSubNode>((*i)->Name());

            // make sure they are the same type
            if ((*i)->Type() != othersChild->Type())
                same = false;
            else if (*(*i) == *othersChild)
            {
                // strip off i
                delete *i;
                children_.erase(i);

                // start loop again
                i = children_.begin();
                continue;
            }
            ++i;
        }

        return same;
    }
}
