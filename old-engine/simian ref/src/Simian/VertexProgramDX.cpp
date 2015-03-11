/************************************************************************/
/*!
\file		VertexProgramDX.cpp
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

#include "Simian/VertexProgramDX.h"
#include "Simian/DataLocation.h"
#include "Simian/LogManager.h"
#include "Simian/GraphicsDevice.h"

namespace Simian
{
    bool loadShader_(const DataLocation& data,
                     const std::string& entryPoint,
                     const std::string& shaderProfile,
                     LPD3DXBUFFER& shader,
                     LPD3DXCONSTANTTABLE& constantTable);

    VertexProgram::VertexProgram(GraphicsDevice* device)
        : ShaderProgram(device),
          shader_(0),
          constantTable_(0)
    {
    }

    //--------------------------------------------------------------------------

    bool VertexProgram::HasParameter( const std::string& name )
    {
        return constantTable_->GetConstantByName(NULL, name.c_str()) ? true : false;
    }

    Simian::ShaderParameter VertexProgram::Parameter( const std::string& name )
    {
        D3DXHANDLE handle = constantTable_->GetConstantByName(NULL, name.c_str());
        return ShaderParameter(handle, constantTable_, &Device());
    }

    //--------------------------------------------------------------------------

    bool VertexProgram::Load( const Simian::DataLocation& data, 
                              const std::string& entryPoint,
                              const std::string& shaderProfile)
    {
        if (!data)
            return false;

        LPD3DXBUFFER shader;
        if (!loadShader_(data, entryPoint, shaderProfile, shader, constantTable_))
            return false;

        // create shader with direct3d device
        HRESULT result = Device().D3DDevice()->CreateVertexShader(
            reinterpret_cast<DWORD*>(shader->GetBufferPointer()), &shader_);

        // check if the result is failure
        if (FAILED(result))
        {
            SWarn("Failed to create shader: " << data.Identifier());
            shader_->Release();
            constantTable_->Release();
            return false;
        }

        // release the shader buffer
        shader->Release();

        // everything went well!
        return true;
    }

    void VertexProgram::Unload()
    {
        // unload the shader program and the constant table
        shader_->Release();
        constantTable_->Release();
    }

    void VertexProgram::Set()
    {
        Device().D3DDevice()->SetVertexShader(shader_);
    }

    void VertexProgram::Unset()
    {
        Device().D3DDevice()->SetVertexShader(NULL);
    }
}

#endif
