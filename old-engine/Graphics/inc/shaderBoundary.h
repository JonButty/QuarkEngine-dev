#ifndef SHADERBOUNDARY_H
#define SHADERBOUNDARY_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_11
#include "shaderBoundaryDX11.h"
#elif ENGINE_GFX == ENGINE_GFX_DX_10
#include "shaderBoundaryDX10.h"
#endif

#endif