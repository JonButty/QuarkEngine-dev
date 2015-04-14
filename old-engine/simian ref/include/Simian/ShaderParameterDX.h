/************************************************************************/
/*!
\file		ShaderParameterDX.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SHADERPARAMETERDX_H_
#define SIMIAN_SHADERPARAMETERDX_H_

#include "SimianPlatform.h"
#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "ShaderParameter.h"

#include <d3dx9.h>

namespace Simian
{
    class GraphicsDevice;

    class SIMIAN_EXPORT ShaderParameter: public ShaderParameterBase
    {
    private:
        D3DXHANDLE handle_;
        LPD3DXCONSTANTTABLE constantTable_;
        GraphicsDevice* device_;
    public:
        ShaderParameter();
        ShaderParameter(D3DXHANDLE handle, LPD3DXCONSTANTTABLE constantTable, GraphicsDevice* device);

        void Set(bool val);
        void Set(const bool* val, u32 size);
        void Set(f32 val);
        void Set(const f32* val, u32 size);
        void Set(const Vector2& val);
        void Set(const Vector3& val);
        void Set(const Vector4& val);
        void Set(const Color& val);
        void Set(const Matrix& val);
        void Set(const Matrix* val, u32 size);

        // secret methods for efficiency (don't use these outside of engine code)
        void setNoTranspose_(const Matrix& val);
        void setNoTranspose_(const Matrix* val, u32 size);
    };
}

#endif

#endif
