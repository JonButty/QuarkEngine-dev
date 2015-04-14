/*****************************************************************************/
/*!
\file		Utility.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_UTILITY_H_
#define SIMIAN_UTILITY_H_

#include "SimianPlatform.h"
#include <string>

#define SUNREFERENCED_PARAMETER(x) (x)

template <class T, class U>
void SFlagSet(T& flags, const U& flag, bool value)
{
    flags = (flags & ~(1 << flag)) | (value << flag);
}

template <class T, class U>
void SFlagSet(T& flag, const U& value)
{
    flag |= (1 << value);
}

template <class T, class U>
void SFlagUnset(T& flag, const U& value)
{
    flag &= ~(1 << value);
}

template <class T, class U>
void SFlagToggle(T& flag, const U& value)
{
    flag ^= (1 << value);
}

template <class T, class U>
bool SIsFlagSet(const T& flag, const U& value)
{
    return (flag & (1 << value)) ? true : false;
}

std::string SIMIAN_EXPORT SToLower(const std::string& input);

#endif
