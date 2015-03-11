/*************************************************************************/
/*!
\file		FileDataSubNode.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_FILEDATASUBNODE_H_
#define SIMIAN_FILEDATASUBNODE_H_

#include "SimianPlatform.h"
#include "FileSubNode.h"

#include <sstream>

namespace Simian
{
    class SIMIAN_EXPORT FileDataSubNode: public FileSubNode
    {
    public:
        std::string value_;
    public:
        bool AsBool() const;
        f32 AsF32() const;
        u32 AsU32() const;
        const std::string& AsString() const;
    public:
        bool operator== ( const FileSubNode& other );
    public:
        template <class T>
        FileDataSubNode(const T& val)
            : FileSubNode(FNT_DATA)
        {
            std::stringstream ss;
            ss.setf(std::ios::boolalpha);
            ss << val;
            value_ = ss.str();
        }

        bool IsBool();
        bool IsFloat();
        bool IsString();
    };
}

#endif
