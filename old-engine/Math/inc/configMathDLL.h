#ifndef CONFIGMATHDLL_H
#define CONFIGMATHDLL_H

#ifdef _MSC_VER
#ifdef MATH_EXPORTS
#define ENGINE_MATH_EXPORT __declspec(dllexport)
#define ENGINE_MATH_IMPORT 
#else
#define ENGINE_MATH_EXPORT __declspec(dllimport)
#define ENGINE_MATH_IMPORT extern
#endif
#endif

#endif