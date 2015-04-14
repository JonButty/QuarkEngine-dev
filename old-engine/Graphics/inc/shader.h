#ifndef SHADER_H
#define SHADER_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10
#include "shaderDX10.h"
#elif ENGINE_GFX == ENGINE_GFX_DX_11
#include "shaderDX11.h"
#endif

#endif