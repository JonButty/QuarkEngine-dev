#ifndef DEBUGAABB_H
#define DEBUGAABB_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10
#include "debugAABBDX10.h"
#elif ENGINE_GFX == ENGINE_GFX_DX_11
#include "debugAABBDX11.h"
#endif

#endif