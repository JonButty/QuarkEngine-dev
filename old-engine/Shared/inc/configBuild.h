#ifndef CONFIGBUILD_H
#define CONFIGBUILD_H

#define ENGINE_BUILD_DEBUG        1
#define ENGINE_BUILD_RELEASE      2
#define ENGINE_BUILD_DEBUG_TEST   3
#define ENGINE_BUILD_RELEASE_TEST 4

#ifdef _DEBUG
#ifdef _TEST
#define ENGINE_BUILD ENGINE_BUILD_DEBUG_TEST
#else
#define ENGINE_BUILD ENGINE_BUILD_DEBUG
#endif

#else
#ifdef _TEST
#define ENGINE_BUILD ENGINE_BUILD_RELEASE_TEST
#else
#define ENGINE_BUILD ENGINE_BUILD_RELEASE
#endif

#endif

#ifdef _LEAK
#define ENGINE_BUILD_LEAK 1
#include "vld.h"
#else
#define ENGINE_BUILD_LEAK 0
#endif

#endif