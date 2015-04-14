/************************************************************************/
/*!
\file		ShaderFloatParameter.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/ShaderFloatParameter.h"
#include "Simian/Debug.h"

#include <memory>

namespace Simian
{
    ShaderFloatParameter::ShaderFloatParameter( const ShaderParameter& param, f32* data, u32 size )
        : ShaderSetParameter(param),
          data_(new f32[size]),
          size_(size)
    {
        SetFloat(data, size);
    }

    ShaderFloatParameter::~ShaderFloatParameter()
    {
        delete [] data_;
    }

    //--------------------------------------------------------------------------

    void ShaderFloatParameter::SetFloat( f32* data, u32 size )
    {
        SAssert(size <= size_, "Too much data for this parameter.");
        std::memcpy(data_, data, size * sizeof(f32));
    }

    void ShaderFloatParameter::Set()
    {
        parameter_.Set(data_, size_);
    }
}
