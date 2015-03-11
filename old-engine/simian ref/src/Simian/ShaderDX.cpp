/************************************************************************/
/*!
\file		ShaderDX.cpp
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
#include "Simian/DataLocation.h"
#include "Simian/LogManager.h"

#include <string>

#include <d3d9.h>
#include <d3dx9.h>

namespace Simian
{
    bool loadShader_(const DataLocation& data,
                     const std::string& entryPoint,
                     const std::string& shaderProfile,
                     LPD3DXBUFFER& shader,
                     LPD3DXCONSTANTTABLE& constantTable)
    {
        LPD3DXBUFFER errors;

        // compile the shader
        std::string shaderProgram(data.Memory(), data.Memory() + data.Size());
        HRESULT result = D3DXCompileShader(shaderProgram.c_str(), shaderProgram.size(), 
            NULL, NULL, entryPoint.c_str(), shaderProfile.c_str(),
            0, &shader, &errors, &constantTable);

        // see if there are any errors
        if (errors && errors->GetBufferSize())
        {
            char* buf = reinterpret_cast<char*>(errors->GetBufferPointer());
            std::string msg(buf, buf + errors->GetBufferSize());
            SWarn("Shader \"" << data.Identifier() << "\" Output: " << msg);
            errors->Release();
        }
        if (FAILED(result))
            return false;

        return true;
    }
}

#endif
