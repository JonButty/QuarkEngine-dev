#ifndef DEBUGSPHERE_H
#define DEBUGSPHERE_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10
#include "debugSphereDX10.h"
#elif ENGINE_GFX == ENGINE_GFX_DX_11
#include "debugSphereDX11.h"
#endif

#endif