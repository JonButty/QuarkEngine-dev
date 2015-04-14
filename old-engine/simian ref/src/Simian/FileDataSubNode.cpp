/*************************************************************************/
/*!
\file		FileDataSubNode.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/FileDataSubNode.h"

#include <algorithm>
#include <cstdio>

namespace Simian
{
    bool FileDataSubNode::AsBool() const
    {
        std::string ret = value_;
        std::transform(ret.begin(), ret.end(), ret.begin(), tolower);
        return ret == "true";
    }

    Simian::f32 FileDataSubNode::AsF32() const
    {
        std::stringstream ss;
        ss << value_;
        f32 ret;
        ss >> ret;
        return ret;
    }

    Simian::u32 FileDataSubNode::AsU32() const
    {
        std::stringstream ss;
        ss << value_;
        u32 ret;
        ss >> ret;
        return ret;
    }

    const std::string& FileDataSubNode::AsString() const
    {
        return value_;
    }

    //--------------------------------------------------------------------------

    bool FileDataSubNode::operator==( const FileSubNode& o )
    {
        // assume other is a datasubnode
        const FileDataSubNode& other = reinterpret_cast<const FileDataSubNode&>(o);

        // check the value
        if (value_ == other.value_)
            return true;
        return false;
    }

    //--------------------------------------------------------------------------

    bool FileDataSubNode::IsBool()
    {
        std::string ret = value_;
        std::transform(ret.begin(), ret.end(), ret.begin(), tolower);
        return ret == "true" || ret == "false";
    }

    bool FileDataSubNode::IsFloat()
    {
        std::stringstream ss;
        ss << value_;
        f32 ret;
        ss >> ret;
        return !ss.fail();
    }

    bool FileDataSubNode::IsString()
    {
        return true;
    }
}
