#ifndef DEBUGSHAPE_H
#define DEBUGSHAPE_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10
#include "debugShapeDX10.h"
#elif ENGINE_GFX == ENGINE_GFX_DX_11
#include "debugShapeDX11.h"
#endif

#endif