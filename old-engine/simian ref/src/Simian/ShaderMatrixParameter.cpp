/************************************************************************/
/*!
\file		ShaderMatrixParameter.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/ShaderMatrixParameter.h"
#include "Simian/Debug.h"

#include <memory>

namespace Simian
{
    ShaderMatrixParameter::ShaderMatrixParameter( const ShaderParameter& parameter, Matrix* data, u32 size )
        : ShaderSetParameter(parameter),
          data_(new Matrix[size]),
          size_(size)
    {
        SetMatrix(data, size);
    }

    ShaderMatrixParameter::~ShaderMatrixParameter()
    {
        delete [] data_;
    }

    //--------------------------------------------------------------------------

    void ShaderMatrixParameter::SetMatrix( Matrix* data, u32 size )
    {
        SAssert(size <= size_, "Too much data for matrix shader parameter.");
        std::memcpy(data_, data, size * sizeof(Matrix));
    }

    void ShaderMatrixParameter::Set()
    {
        parameter_.Set(data_, size_);
    }
}
