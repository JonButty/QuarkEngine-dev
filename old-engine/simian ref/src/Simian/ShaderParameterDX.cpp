/************************************************************************/
/*!
\file		ShaderParameterDX.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/SimianPlatform.h"
#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "Simian/ShaderParameterDX.h"
#include "Simian/GraphicsDevice.h"

namespace Simian
{
    static const u32 MAX_MATRIX_ARRAY = 128;
    static Matrix matrixArray_[MAX_MATRIX_ARRAY];

    ShaderParameter::ShaderParameter()
        : handle_(0),
          constantTable_(0),
          device_(0)
    {
    }

    ShaderParameter::ShaderParameter( D3DXHANDLE handle, 
                                      LPD3DXCONSTANTTABLE constantTable, 
                                      GraphicsDevice* device)
        : handle_(handle),
          constantTable_(constantTable),
          device_(device)
    {
    }

    //--------------------------------------------------------------------------

    void ShaderParameter::Set( bool val )
    {
        constantTable_->SetBool(device_->D3DDevice(), handle_, val);
    }

    void ShaderParameter::Set( const bool* val, u32 size )
    {
        constantTable_->SetBoolArray(device_->D3DDevice(), handle_, 
                                     reinterpret_cast<CONST BOOL*>(val), size);
    }

    void ShaderParameter::Set( f32 val )
    {
        constantTable_->SetFloat(device_->D3DDevice(), handle_, val);
    }

    void ShaderParameter::Set( const f32* val, u32 size )
    {
        constantTable_->SetFloatArray(device_->D3DDevice(), handle_, val, size);
    }

    void ShaderParameter::Set( const Vector2& val )
    {
        // risky, but for speed good (low level stuff)
        constantTable_->SetFloatArray(device_->D3DDevice(), handle_, 
            reinterpret_cast<const FLOAT*>(&val), 2);
    }

    void ShaderParameter::Set( const Vector3& val )
    {
        // risky, but for speed good (low level stuff)
        constantTable_->SetFloatArray(device_->D3DDevice(), handle_, 
            reinterpret_cast<const FLOAT*>(&val), 3);
    }

    void ShaderParameter::Set( const Vector4& val )
    {
        // risky, but for speed good (low level stuff)
        constantTable_->SetFloatArray(device_->D3DDevice(), handle_, 
            reinterpret_cast<const FLOAT*>(&val), 4);
    }

    void ShaderParameter::Set( const Color& val )
    {
        // risky, but for speed good (low level stuff)
        constantTable_->SetFloatArray(device_->D3DDevice(), handle_, 
            reinterpret_cast<const FLOAT*>(&val), 4);
    }

    void ShaderParameter::Set( const Matrix& val )
    {
        // risky, but necessary for speed
        constantTable_->SetMatrix(device_->D3DDevice(), handle_, 
            reinterpret_cast<const D3DXMATRIX*>(val.Transposed()[0]));
    }

    void ShaderParameter::Set( const Matrix* val, u32 size )
    {
        // assert if size is more than max size
        SAssert(size < MAX_MATRIX_ARRAY, "Too many matrices for shader parameter.");

        // this one is tricky, we need to transpose all the matrices
        // copy all matrices to the matrix array
        std::memcpy(matrixArray_, val, size * sizeof(Matrix));
        
        // transpose the arrays worth
        for (u32 i = 0; i < size; ++i)
            matrixArray_[i].Transpose();

        // set matrix array
        constantTable_->SetMatrixArray(device_->D3DDevice(), handle_,
            reinterpret_cast<const D3DXMATRIX*>(matrixArray_), size);
    }

    void ShaderParameter::setNoTranspose_( const Matrix& val )
    {
        // risky, but necessary for speed
        constantTable_->SetMatrix(device_->D3DDevice(), handle_, 
            reinterpret_cast<const D3DXMATRIX*>(&val));
    }

    void ShaderParameter::setNoTranspose_( const Matrix* val, u32 size )
    {
        // set matrix array
        constantTable_->SetMatrixArray(device_->D3DDevice(), handle_,
            reinterpret_cast<const D3DXMATRIX*>(val), size);
    }
}

#endif
