#ifndef GFXAPI_H
#define GFXAPI_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_11
#include "gfxAPIDX11.h"
#elif ENGINE_GFX == ENGINE_GFX_DX_10
#include "gfxAPIDX10.h"
#endif

#endif