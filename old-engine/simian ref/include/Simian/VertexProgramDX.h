/************************************************************************/
/*!
\file		VertexProgramDX.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_VERTEXPROGRAMDX_H_
#define SIMIAN_VERTEXPROGRAMDX_H_

#include "SimianPlatform.h"
#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "ShaderProgram.h"
#include "ShaderParameterDX.h"

#include <d3d9.h>
#include <d3dx9.h>

namespace Simian
{
    class SIMIAN_EXPORT VertexProgram: public ShaderProgram
    {
    private:
        LPDIRECT3DVERTEXSHADER9 shader_;
        LPD3DXCONSTANTTABLE constantTable_;
    public:
        bool HasParameter(const std::string& name);
        ShaderParameter Parameter(const std::string& name);
    public:
        VertexProgram(GraphicsDevice* device);

        bool Load(const Simian::DataLocation& data, 
                  const std::string& entryPoint = "main",
                  const std::string& shaderProfile = "vs_1_1");
        void Unload();

        void Set();
        void Unset();
    };
}

#endif

#endif
