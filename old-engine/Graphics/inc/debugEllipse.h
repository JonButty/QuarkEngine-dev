#ifndef DEBUGELLIPSE_H
#define DEBUGELLIPSE_H

#include "configGFX.h"
#if ENGINE_GFX == ENGINE_GFX_DX_10
#include "debugEllipseDX10.h"
#elif ENGINE_GFX == ENGINE_GFX_DX_11
#include "debugEllipseDX11.h"
#endif

#endif