/*************************************************************************/
/*!
\file		FileSubNode.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_FILESUBNODE_H_
#define SIMIAN_FILESUBNODE_H_

#include "SimianPlatform.h"

#include <string>

namespace Simian
{
    class SIMIAN_EXPORT FileSubNode
    {
    public:
        enum FileSubNodeType
        {
            FNT_OBJECT,
            FNT_DATA,
            FNT_ARRAY
        };
    private:
        std::string name_;
        FileSubNodeType type_;
    public:
        virtual bool operator== (const FileSubNode& other) = 0;
    public:
        FileSubNode(FileSubNodeType type);
        virtual ~FileSubNode();

        std::string Name() const;
        void Name(std::string val);

        FileSubNode::FileSubNodeType Type() const;
    };
}

#endif
