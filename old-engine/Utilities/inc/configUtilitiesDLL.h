#ifndef CONFIGUTILITIESDLL_H
#define CONFIGUTILITIESDLL_H

#ifdef _MSC_VER
#ifdef UTILITIES_EXPORTS
#define ENGINE_UTILITIES_EXPORT __declspec(dllexport)
#define ENGINE_UTILITIES_IMPORT 
#else
#define ENGINE_UTILITIES_EXPORT __declspec(dllimport)
#define ENGINE_UTILITIES_IMPORT extern
#endif
#endif

#endif