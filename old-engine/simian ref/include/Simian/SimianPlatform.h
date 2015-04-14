/*****************************************************************************/
/*!
\file		SimianPlatform.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_SIMIANPLATFORM_H_
#define SIMIAN_SIMIANPLATFORM_H_

#define SIMIAN_PLATFORM_WIN32	1
#define SIMIAN_PLATFORM_LINUX	2
#define SIMIAN_PLATFORM_OSX		3

#define SIMIAN_COMPILER_MSVC	1
#define SIMIAN_COMPILER_GCC		2

#define SIMIAN_DEBUG_DEBUG 1
#define SIMIAN_DEBUG_RELEASE 2

#define SIMIAN_GRAPHICS_DX 1
#define SIMIAN_GRAPHICS_GL 2

#define SIMIAN_VIDEO_THEORA 1

#define SIMIAN_DATAFORMAT_JSON 1
#define SIMIAN_DATAFORMAT_XML 2

// Release information
#define SIMIAN_ENGINE_NAME "Simian Engine"
#define SIMIAN_ENGINE_VERSION "0.0.5-rc"
#define SIMIAN_ENGINE_REVISION_NAME "Acapulco"

// Platform Check
#ifdef _WIN32
#define SIMIAN_PLATFORM SIMIAN_PLATFORM_WIN32
#elif __APPLE__
#define SIMIAN_PLATFORM SIMIAN_PLATFORM_OSX
#elif linux
#define SIMIAN_PALTFORM SIMIAN_PLATFORM_LINUX
#endif

// Compiler Check
#ifdef _MSC_VER
#define SIMIAN_COMPILER SIMIAN_COMPILER_MSVC
#elif __GNUC__
#define SIMIAN_COMPILER SIMIAN_COMPILER_GCC
#endif

// Debug/Release Check
#ifdef _DEBUG
#define SIMIAN_DEBUG SIMIAN_DEBUG_DEBUG
#elif NDEBUG
#define SIMIAN_DEBUG SIMIAN_DEBUG_RELEASE
#else
#define SIMIAN_DEBUG SIMIAN_DEBUG_RELEASE
#endif

// Import/Export Check
#if SIMIAN_COMPILER == SIMIAN_COMPILER_MSVC
#ifdef SIMIANENGINE_EXPORTS
#define SIMIAN_EXPORT __declspec(dllexport)
#define SIMIAN_IMPORT 
#else
#define SIMIAN_EXPORT __declspec(dllimport)
#define SIMIAN_IMPORT extern
#endif
#else
#define SIMIAN_EXPORT 
#define SIMIAN_IMPORT  
#endif

// Graphics/Platform Check
#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32
#define SIMIAN_GRAPHICS_DEFAULT SIMIAN_GRAPHICS_DX
#else
#define SIMIAN_GRAPHICS_DEFAULT SIMIAN_GRAPHICS_GL
#endif

// Markers
#define S_NIMPL

#include "SimianTypes.h"
#include "SimianConfig.h"
#include "SimianTypes.h"
#endif
