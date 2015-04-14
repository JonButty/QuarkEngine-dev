/************************************************************************/
/*!
\file		ShaderSetParameter.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SHADERSETPARAMETER_H_
#define SIMIAN_SHADERSETPARAMETER_H_

#include "SimianPlatform.h"
#include "ShaderParameter.h"

#include <string>

namespace Simian
{
    // abstract class for a shader parameter setter (required for setting 
    // constants before using shaders)
    class SIMIAN_EXPORT ShaderSetParameter
    {
    protected:
        ShaderParameter parameter_;
    public:
        ShaderSetParameter(const ShaderParameter& parameter)
            : parameter_(parameter) {}
        virtual ~ShaderSetParameter() {}

        virtual void Set() = 0;
    };
}

#endif
