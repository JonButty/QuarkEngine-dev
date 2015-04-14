/************************************************************************/
/*!
\file		ShaderBoolParameter.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/ShaderBoolParameter.h"
#include "Simian/Debug.h"

#include <memory>

namespace Simian
{
    ShaderBoolParameter::ShaderBoolParameter( const ShaderParameter& param, bool* data, u32 size )
        : ShaderSetParameter(param),
          data_(new bool[size]),
          size_(size)
    {
        SetBool(data, size);
    }

    ShaderBoolParameter::~ShaderBoolParameter()
    {
        delete [] data_;
    }

    //--------------------------------------------------------------------------

    void ShaderBoolParameter::SetBool( bool* data, u32 size )
    {
        SAssert(size <= size_, "Too much data for the bool shader parameter.");
        std::memcpy(data_, data, size * sizeof(bool));
    }

    void ShaderBoolParameter::Set()
    {
        parameter_.Set(data_, size_);
    }
}
