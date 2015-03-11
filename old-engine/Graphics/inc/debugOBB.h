#ifndef DEBUGOBB_H
#define DEBUGOBB_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10
#include "debugOBBDX10.h"
#elif ENGINE_GFX == ENGINE_GFX_DX_11
#include "debugOBBDX11.h"
#endif

#endif