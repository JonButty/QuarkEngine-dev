/************************************************************************/
/*!
\file		ShaderParameter.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SHADERPARAMETER_H_
#define SIMIAN_SHADERPARAMETER_H_

#include "SimianPlatform.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Color.h"
#include "Matrix.h"

namespace Simian
{
    class SIMIAN_EXPORT ShaderParameterBase
    {
    public:
        virtual ~ShaderParameterBase() {}

        S_NIMPL void Set(bool) {}
        S_NIMPL void Set(const bool*, u32) {}
        S_NIMPL void Set(f32) {}
        S_NIMPL void Set(const f32*, u32) {}
        S_NIMPL void Set(const Vector2&) {}
        S_NIMPL void Set(const Vector3&) {}
        S_NIMPL void Set(const Vector4&) {}
        S_NIMPL void Set(const Color&) {}
        S_NIMPL void Set(const Matrix&) {}
        S_NIMPL void Set(const Matrix*, u32) {}
    };
}

#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "ShaderParameterDX.h"
#elif SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_GL
#include "ShaderParameterGL.h"
#endif

#endif
