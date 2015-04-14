/************************************************************************/
/*!
\file		FragmentProgramDX.cpp
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
#include "Simian/FragmentProgramDX.h"
#include "Simian/GraphicsDevice.h"
#include "Simian/LogManager.h"
#include "Simian/DataLocation.h"

namespace Simian
{
    bool loadShader_(const DataLocation& data,
                     const std::string& entryPoint,
                     const std::string& shaderProfile,
                     LPD3DXBUFFER& shader,
                     LPD3DXCONSTANTTABLE& constantTable);

    FragmentProgram::FragmentProgram( GraphicsDevice* device )
        : ShaderProgram(device),
          shader_(0),
          constantTable_(0)
    {
    }

    //--------------------------------------------------------------------------

    bool FragmentProgram::HasParameter( const std::string& name )
    {
        return constantTable_->GetConstantByName(NULL, name.c_str()) ? true : false;
    }

    Simian::ShaderParameter FragmentProgram::Parameter( const std::string& name )
    {
        D3DXHANDLE handle = constantTable_->GetConstantByName(NULL, name.c_str());
        return ShaderParameter(handle, constantTable_, &Device());
    }

    //--------------------------------------------------------------------------

    bool FragmentProgram::Load( const Simian::DataLocation& data, 
                                const std::string& entryPoint, 
                                const std::string& shaderProfile )
    {
        if (!data)
            return false;

        LPD3DXBUFFER shader;
        if (!loadShader_(data, entryPoint, shaderProfile, shader, constantTable_))
            return false;

        HRESULT result = Device().D3DDevice()->CreatePixelShader(
            reinterpret_cast<DWORD*>(shader->GetBufferPointer()), &shader_);

        if (FAILED(result))
        {
            SWarn("Failed to create shader: " << data.Identifier());
            shader_->Release();
            constantTable_->Release();
            return false;
        }

        shader->Release();
        
        return true;
    }

    void FragmentProgram::Unload()
    {
        shader_->Release();
        constantTable_->Release();
    }

    void FragmentProgram::Set()
    {
        Device().D3DDevice()->SetPixelShader(shader_);
    }

    void FragmentProgram::Unset()
    {
        Device().D3DDevice()->SetPixelShader(NULL);
    }
}

#endif
