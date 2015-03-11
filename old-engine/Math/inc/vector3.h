#ifndef VECTOR3_H
#define VECTOR3_H

#include "configMath.h"
#if ENGINE_MATH == ENGINE_MATH_SSE
#include "vector3SSE.h"
#elif ENGINE_MATH == ENGINE_MATH_NATIVE
#include "vector3Native.h"
#elif ENGINE_MATH == ENGINE_MATH_DIRECTXMATH
#include "vector3DirectXMath.h"
#elif ENGINE_MATH == ENGINE_MATH_D3DXMATH
#include "vector3D3DXMath.h"
#endif

#endif