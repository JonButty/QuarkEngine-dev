#ifndef MESH_H
#define MESH_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10
#include "meshDX10.h"
#elif ENGINE_GFX == ENGINE_GFX_DX_11
#include "meshDX11.h"
#endif

#endif