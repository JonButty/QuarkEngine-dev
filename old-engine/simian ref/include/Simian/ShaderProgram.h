/************************************************************************/
/*!
\file		ShaderProgram.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SHADERPROGRAM_H_
#define SIMIAN_SHADERPROGRAM_H_

#include "SimianPlatform.h"
#include "ShaderParameter.h"

#include <string>
#include <map>

namespace Simian
{
    class DataLocation;
    class GraphicsDevice;

    class SIMIAN_EXPORT ShaderProgram
    {
    private:
        GraphicsDevice* device_;
    public:
        GraphicsDevice& Device() const { return *device_; }

        S_NIMPL bool HasParameter(const std::string& name) { name; return false; }
        S_NIMPL ShaderParameter Parameter(const std::string& name) { name; return ShaderParameter(); }
    public:
        ShaderProgram(GraphicsDevice* device)
            : device_(device) {}
        virtual ~ShaderProgram() {}

        S_NIMPL virtual bool Load(const Simian::DataLocation&, 
                                  const std::string& entryPoint = "main",
                                  const std::string& shaderProfile = "")
        { entryPoint; shaderProfile; return false; }
        S_NIMPL virtual void Unload() {}

        S_NIMPL virtual void Set() {}
        S_NIMPL virtual void Unset() {}

        ShaderParameter operator[](const std::string& index) { return Parameter(index); }
    };
}

#if SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_DX
#include "VertexProgramDX.h"
#include "FragmentProgramDX.h"
#elif SIMIAN_GRAPHICS == SIMIAN_GRAPHICS_GL
#include "VertexProgramGL.h"
#include "FragmentProgramGL.h"
#endif

#endif
