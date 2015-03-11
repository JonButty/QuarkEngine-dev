/************************************************************************/
/*!
\file		FragmentProgramDX.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_FRAGMENTSHADERDX_H_
#define SIMIAN_FRAGMENTSHADERDX_H_

#include "SimianPlatform.h"
#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "ShaderProgram.h"
#include "ShaderParameter.h"

#include <d3d9.h>
#include <d3dx9.h>

namespace Simian
{
    class SIMIAN_EXPORT FragmentProgram: public ShaderProgram
    {
    private:
        LPDIRECT3DPIXELSHADER9 shader_;
        LPD3DXCONSTANTTABLE constantTable_;
    public:
        bool HasParameter(const std::string& name);
        ShaderParameter Parameter(const std::string& name);
    public:
        FragmentProgram(GraphicsDevice* device);

        bool Load(const Simian::DataLocation&, 
                  const std::string& entryPoint = "main", 
                  const std::string& shaderProfile = "ps_2_0");
        void Unload();

        void Set();
        void Unset();
    };
}

#endif

#endif
