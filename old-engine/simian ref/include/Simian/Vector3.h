/*****************************************************************************/
/*!
\file		Vector3.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_VECTOR3_H_
#define SIMIAN_VECTOR3_H_

#include "SimianPlatform.h"

#if SIMIAN_SSE_SUPPORT == 1
#include "Vector3SSE.h"
#else
#include "Vector3Native.h"
#endif

#endif
