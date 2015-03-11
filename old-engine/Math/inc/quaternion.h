#ifndef QUATERNION_H
#define QUATERNION_H

#include "configMath.h"
#if ENGINE_MATH == ENGINE_MATH_SSE
#include "quaternionSSE.h"
#elif ENGINE_MATH == ENGINE_MATH_NATIVE
#include "quaternionNative.h"
#elif ENGINE_MATH == ENGINE_MATH_DIRECTXMATH
#include "quaternionDirectXMath.h"
#elif ENGINE_MATH == ENGINE_MATH_D3DXMATH
#include "quaternionD3DXMath.h"
#endif

#endif