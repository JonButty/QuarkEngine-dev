/************************************************************************/
/*!
\file		Utility.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Utility.h"

#include <string>
#include <algorithm>

std::string SToLower(const std::string& input)
{
    std::string ret = input;
    std::transform(ret.begin(), ret.end(), ret.begin(), tolower);
    return ret;
}
