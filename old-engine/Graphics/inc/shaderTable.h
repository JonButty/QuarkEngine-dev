#ifndef SHADERTABLE_H
#define SHADERTABLE_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_11
#include "shaderTableDX11.h"
#elif ENGINE_GFX == ENGINE_GFX_DX_10
#include "shaderTableDX10.h"
#endif

#endif