/************************************************************************/
/*!
\file		ShaderBoolParameter.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	November 1, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SHADERBOOLPARAMETER_H_
#define SIMIAN_SHADERBOOLPARAMETER_H_

#include "SimianPlatform.h"
#include "ShaderSetParameter.h"

namespace Simian
{
    class SIMIAN_EXPORT ShaderBoolParameter: public ShaderSetParameter
    {
    private:
        bool* data_;
        u32 size_;
    private:
        // unimplemented copy and assignment
        ShaderBoolParameter(const ShaderBoolParameter&);
        ShaderBoolParameter& operator=(const ShaderBoolParameter&);
    public:
        ShaderBoolParameter(const ShaderParameter& param, bool* data, u32 size);
        ~ShaderBoolParameter();

        void SetBool(bool* data, u32 size);
        void Set();
    };
}

#endif
