/*************************************************************************/
/*!
\file		Console.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_CONSOLE_H_
#define DESCENSION_CONSOLE_H_

//#define RELEASE_CONSOLE

#include "Simian/SimianPlatform.h"
#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32 && (defined(_DEBUG) || defined(RELEASE_CONSOLE))

namespace Descension
{
    class Console
    {
    public:
        static void Show();
    };
}

#define OpenConsoleWindow() Descension::Console::Show();

#else

#define OpenConsoleWindow()

#endif

#endif
