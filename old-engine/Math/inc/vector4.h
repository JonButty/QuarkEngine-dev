#ifndef VECTOR4_H
#define VECTOR4_H

#include "configMath.h"
#if ENGINE_MATH == ENGINE_MATH_SSE
#include "vector4SSE.h"
#elif ENGINE_MATH == ENGINE_MATH_NATIVE
#include "vector4Native.h"
#elif ENGINE_MATH == ENGINE_MATH_DIRECTXMATH
#include "vector4DirectXMath.h"
#elif ENGINE_MATH == ENGINE_MATH_D3DXMATH
#include "vector4D3DXMath.h"
#endif

#endif