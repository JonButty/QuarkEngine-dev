#ifndef SHADERPHONG_H
#define SHADERPHONG_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_11
#include "shaderDX11.h"
#elif ENGINE_GFX == ENGINE_GFX_DX_10
#include "shaderDX10.h"
#endif

#endif