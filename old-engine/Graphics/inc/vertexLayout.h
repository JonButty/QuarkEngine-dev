#ifndef VERTEXLAYOUT_H
#define VERTEXLAYOUT_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_11
#include "vertexLayoutDX11.h"
#elif ENGINE_GFX == ENGINE_GFX_DX_10
#include "vertexLayoutDX10.h"
#endif

#endif