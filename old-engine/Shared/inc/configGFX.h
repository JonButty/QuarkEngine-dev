#ifndef CONFIGGFX_H
#define CONFIGGFX_H

#define ENGINE_GFX_DX_10 1 
#define ENGINE_GFX_DX_11 2 
// Other compatible graphics API

#if ENGINE_OS == ENGINE_OS_WINDOWS

#if ENGINE_OS_VER == ENGINE_OS_VER_WINDOWS8
#define ENGINE_GFX ENGINE_GFX_DX_11
#else
#define ENGINE_GFX ENGINE_GFX_DX_10
#endif

#else
#define ENGINE_GFX ENGINE_GFX_GL
#endif

#define ENGINE_GFX_VSYNC 0
#define ENGINE_GFX_FULLSCREEN 0
#define ENGINE_GFX_WIDTH 1920
#define ENGINE_GFX_HEIGHT 1080

// Default camera info
#define ENGINE_GFX_NEAR 0.1f
#define ENGINE_GFX_FAR 1000.0f
#define ENGINE_GFX_FOV 45

#endif