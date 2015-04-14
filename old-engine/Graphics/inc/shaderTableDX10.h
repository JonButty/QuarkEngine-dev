#ifndef SHADERTABLEDX10_H
#define SHADERTABLEDX10_H

#include "shaderTypes.h"

#include <string>

namespace GFX
{
    static const std::wstring SHADER_TABLE [ST_TOTAL] = 
    {
        std::wstring(L"default.fx"),
        std::wstring(L"boundary.fx"),
        std::wstring(L"phong.fx"),
        std::wstring(L"normals.fx"),
    };
}

#endif