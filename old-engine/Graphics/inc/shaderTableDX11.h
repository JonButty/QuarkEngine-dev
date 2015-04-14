#ifndef SHADERTABLEDX11_H
#define SHADERTABLEDX11_H

#include "shaderTypes.h"

#include <utility>
#include <string>

namespace GFX
{
    static const std::pair<std::wstring,std::wstring> SHADER_TABLE [ST_TOTAL] = 
    {
        std::pair<std::wstring,std::wstring>(L"default.vs",L"default.ps"),
        std::pair<std::wstring,std::wstring>(L"phong.vs",L"phong.ps")
    };
}

#endif