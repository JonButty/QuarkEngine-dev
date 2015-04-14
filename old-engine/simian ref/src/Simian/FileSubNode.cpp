/*************************************************************************/
/*!
\file		FileSubNode.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/FileSubNode.h"

namespace Simian
{
    FileSubNode::FileSubNode(FileSubNodeType type)
        : type_(type)
    {
    }

    FileSubNode::~FileSubNode()
    {
    }

    //--------------------------------------------------------------------------

    std::string FileSubNode::Name() const
    {
        return name_;
    }

    void FileSubNode::Name( std::string val )
    {
        name_ = val;
    }

    FileSubNode::FileSubNodeType FileSubNode::Type() const
    {
        return type_;
    }
}
