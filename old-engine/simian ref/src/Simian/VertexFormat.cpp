/************************************************************************/
/*!
\file		VertexFormat.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/VertexFormat.h"

namespace Simian
{
    VertexFormat::VertexFormat()
        : size_(0)
    {
    }

    //--------------------------------------------------------------------------

    const std::vector<VertexAttribute>& VertexFormat::Attributes() const
    {
        return attributes_;
    }

    Simian::u32 VertexFormat::Size() const
    {
        return size_;
    }

    //--------------------------------------------------------------------------

    void VertexFormat::AddAttribute( VertexAttribute::VertexUsage usage, VertexAttribute::VertexType type, u32 index, u32 offset )
    {
        VertexAttribute attribute;
        attribute.Offset = offset;
        attribute.Type = type;
        attribute.Usage = usage;
        attribute.Index = index;
        attributes_.push_back(attribute);

        size_ += offset + attribute.Size();
    }
}
