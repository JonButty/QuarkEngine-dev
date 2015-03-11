/************************************************************************/
/*!
\file		ShaderFloatParameter.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SHADERFLOATPARAMETER_H_
#define SIMIAN_SHADERFLOATPARAMETER_H_

#include "SimianPlatform.h"
#include "ShaderSetParameter.h"

namespace Simian
{
    class SIMIAN_EXPORT ShaderFloatParameter: public ShaderSetParameter
    {
    private:
        f32* data_;
        unsigned size_;
    private:
        // unimplemented copy and assignment
        ShaderFloatParameter(const ShaderFloatParameter&);
        ShaderFloatParameter& operator=(const ShaderFloatParameter&);
    public:
        ShaderFloatParameter(const ShaderParameter& parameter, f32* data, u32 size);
        ~ShaderFloatParameter();

        void SetFloat(f32* data, u32 size);
        void Set();
    };
}

#endif
