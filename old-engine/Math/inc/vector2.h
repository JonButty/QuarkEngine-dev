#ifndef VECTOR2_H
#define VECTOR2_H

#include "configMath.h"
#if ENGINE_MATH == ENGINE_MATH_SSE
#include "vector2SSE.h"
#elif ENGINE_MATH == ENGINE_MATH_NATIVE
#include "vector2Native.h"
#elif ENGINE_MATH == ENGINE_MATH_DIRECTXMATH
#include "vector2DirectXMath.h"
#elif ENGINE_MATH == ENGINE_MATH_D3DXMATH
#include "vector2D3DXMath.h"
#endif

#endif