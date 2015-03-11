#ifndef MATRIX4_H
#define MATRIX4_H

#include "configMath.h"
#if ENGINE_MATH == ENGINE_MATH_SSE
#include "matrix4SSE.h"
#elif ENGINE_MATH == ENGINE_MATH_NATIVE
#include "matrix4Native.h"
#elif ENGINE_MATH == ENGINE_MATH_DIRECTXMATH
#include "matrix4DirectXMath.h"
#elif ENGINE_MATH == ENGINE_MATH_D3DXMATH
#include "matrix4D3DXMath.h"
#endif

#endif