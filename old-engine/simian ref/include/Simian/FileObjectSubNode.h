/*************************************************************************/
/*!
\file		FileObjectSubNode.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_FILEOBJECTSUBNODE_H_
#define SIMIAN_FILEOBJECTSUBNODE_H_

#include "SimianPlatform.h"
#include "FileSubNode.h"

#include <set>
#include <algorithm>

namespace Simian
{
    class FileArraySubNode;
    class FileDataSubNode;

    struct SIMIAN_EXPORT FileObjectSort: std::binary_function<FileSubNode*, FileSubNode*, bool>
    {
        bool operator()(FileSubNode* a, FileSubNode* b) const
        {
            return a->Name() < b->Name();
        }
    };

    class SIMIAN_EXPORT FileObjectFind: std::unary_function<FileSubNode*, bool>
    {
    private:
        std::string name_;
    public:
        FileObjectFind(const std::string& name)
            : name_(name) {}

        bool operator()(FileSubNode* a) const
        {
            return a->Name() == name_;                
        }
    };

    class SIMIAN_EXPORT FileObjectSubNode: public FileSubNode
    {
    public:
        typedef std::set<FileSubNode*, FileObjectSort>::iterator Iterator;
        typedef std::set<FileSubNode*, FileObjectSort>::const_iterator ConstIterator;
    private:
        std::set<FileSubNode*, FileObjectSort> children_;
    public:
        Iterator Begin();
        Iterator End();
        ConstIterator Begin() const;
        ConstIterator End() const;
        Simian::u32 Count() const;
    public:
        FileSubNode* operator[](const std::string& name);
        const FileSubNode* operator[](const std::string& name) const;

        FileObjectSubNode& operator=(const FileObjectSubNode& copy);

        bool operator==( const FileSubNode& other );
    public:
        FileObjectSubNode();
        FileObjectSubNode(const FileObjectSubNode& copy);
        ~FileObjectSubNode();

        void AddChild(FileSubNode* node);
        void RemoveChild(FileSubNode* node);
        void RemoveChild(const std::string& name);
        
        template <class T>
        T* Get(const std::string& name)
        {
            return reinterpret_cast<T*>((*this)[name]);
        }

        template <class T>
        const T* Get(const std::string& name) const
        {
            return reinterpret_cast<const T*>((*this)[name]);
        }

        FileObjectSubNode* Object(const std::string& name);
        const FileObjectSubNode* Object(const std::string& name) const;
        
        FileArraySubNode* Array(const std::string& name);
        const FileArraySubNode* Array(const std::string& name) const;

        FileDataSubNode* Data(const std::string& name);
        const FileDataSubNode* Data(const std::string& name) const;
        void Data(const std::string& name, bool& out, bool default = false) const;
        void Data(const std::string& name, float& out, float default = 0.0f) const;
        void Data(const std::string& name, std::string& out, std::string default = "") const;
        template <typename T>
        void Data(const std::string& name, T& out, const T& default = 0) const
        {
            const FileDataSubNode* data = Data(name);
            out = data ? (T)data->AsF32() : default;
        }
        

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

        // strip similarities
        bool Diff(const FileObjectSubNode& other);
    };
}

#endif
