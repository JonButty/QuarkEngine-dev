/*************************************************************************/
/*!
\file		FileArraySubNode.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_FILEARRAYSUBNODE_H_
#define SIMIAN_FILEARRAYSUBNODE_H_

#include "SimianPlatform.h"
#include "FileSubNode.h"

#include <vector>

namespace Simian
{
    class FileObjectSubNode;
    class FileDataSubNode;

    class SIMIAN_EXPORT FileArraySubNode: public FileSubNode
    {
    private:
        std::vector<FileSubNode*> children_;
    public:
        u32 Size() const;
    public:
        FileSubNode* operator[](u32 index);
        const FileSubNode* operator[](u32 index) const;

        FileArraySubNode& operator=(const FileArraySubNode& copy);

        bool operator== ( const FileSubNode& other );
    public:
        FileArraySubNode();
        FileArraySubNode(const FileArraySubNode& copy);
        ~FileArraySubNode();

        template <class T>
        const T* Get(u32 index) const
        {
            return reinterpret_cast<T*>(children_[index]);
        }

        template <class T>
        T* Get(u32 index)
        {
            return reinterpret_cast<T*>(children_[index]);
        }

        void AddChild(FileSubNode* node);

        FileObjectSubNode* Object(u32 name);
        const FileObjectSubNode* Object(u32 name) const;

        FileArraySubNode* Array(u32 name);
        const FileArraySubNode* Array(u32 name) const;

        FileDataSubNode* Data(u32 name);
        const FileDataSubNode* Data(u32 name) const;

        FileObjectSubNode* AddObject(const std::string& name);
        FileArraySubNode* AddArray(const std::string& name);

        template <class T>
        FileDataSubNode* AddData(const std::string& name, const T& val)
        {
            FileDataSubNode* object = new FileDataSubNode(val);
            object->Name(name);
            AddChild(object);
            return object;
        }
    };
}

#endif
