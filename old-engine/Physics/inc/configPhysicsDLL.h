#ifndef CONFIGPHYSICSDLL_H
#define CONFIGPHYSICSDLL_H

#ifdef _MSC_VER
#ifdef PHYSICS_EXPORTS
#define ENGINE_PHYSICS_EXPORT __declspec(dllexport)
#define ENGINE_PHYSICS_IMPORT 
#else
#define ENGINE_PHYSICS_EXPORT __declspec(dllimport)
#define ENGINE_PHYSICS_IMPORT extern
#endif
#endif

#endif