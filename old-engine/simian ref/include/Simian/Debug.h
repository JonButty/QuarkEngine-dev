/*****************************************************************************/
/*!
\file		Debug.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_DEBUG_H_
#define SIMIAN_DEBUG_H_

#include "SimianPlatform.h"

#if SIMIAN_DEBUG == SIMIAN_DEBUG_DEBUG & SIMIAN_ASSERT == 1
#define SAssert(expr, msg) Simian::OS::Instance().Assert(expr, L ## msg)
#else
#define SAssert(expr, msg)
#endif

#include "OS.h"

#endif
