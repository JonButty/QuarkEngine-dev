#ifndef CONFIGCOREDLL_H
#define CONFIGCOREDLL_H

#ifdef _MSC_VER
#ifdef CORE_EXPORTS
#define ENGINE_CORE_EXPORT __declspec(dllexport)
#define ENGINE_CORE_IMPORT 
#else
#define ENGINE_CORE_EXPORT __declspec(dllimport)
#define ENGINE_CORE_IMPORT extern
#endif
#endif

#endif