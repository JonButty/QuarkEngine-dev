#ifndef CONFIGGRAPHICSDLL_H
#define CONFIGGRAPHICSDLL_H

#ifdef _MSC_VER
#ifdef GRAPHICS_EXPORTS
#define ENGINE_GRAPHICS_EXPORT __declspec(dllexport)
#define ENGINE_GRAPHICS_IMPORT 
#else
#define ENGINE_GRAPHICS_EXPORT __declspec(dllimport)
#define ENGINE_GRAPHICS_IMPORT extern
#endif
#endif

#endif