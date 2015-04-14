#ifndef CONFIGCOMPILER_H
#define CONFIGCOMPILER_H

#define ENGINE_COMPILER_MSVC 1
// Other compatible compilers

#ifdef _MSC_VER
#define ENGINE_COMPILER ENGINE_COMPILER_MSVC
#endif
// Other compatible compilers

#endif