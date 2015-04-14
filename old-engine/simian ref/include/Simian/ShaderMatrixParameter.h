/************************************************************************/
/*!
\file		ShaderMatrixParameter.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SHADERMATRIXPARAMETER_H_
#define SIMIAN_SHADERMATRIXPARAMETER_H_

#include "SimianPlatform.h"
#include "Matrix.h"
#include "ShaderSetParameter.h"

namespace Simian
{
    class ShaderMatrixParameter: public ShaderSetParameter
    {
    private:
        Matrix* data_;
        u32 size_;
    private:
        // unimplemented copy and assignment
        ShaderMatrixParameter(const ShaderMatrixParameter&);
        ShaderMatrixParameter& operator=(const ShaderMatrixParameter&);
    public:
        ShaderMatrixParameter(const ShaderParameter& parameter, Matrix* data, u32 size);
        ~ShaderMatrixParameter();

        void SetMatrix(Matrix* data, u32 size);
        void Set();
    };
}

#endif
