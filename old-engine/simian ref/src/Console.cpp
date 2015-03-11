/*************************************************************************/
/*!
\file		Console.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/SimianPlatform.h"
#include "Console.h"
#if SIMIAN_PLATFORM == SIMIAN_PLATFORM_WIN32 && (defined(_DEBUG) || defined(RELEASE_CONSOLE))

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h>
#include <fcntl.h>

#include <cstdio>
#include <iostream>

namespace Descension
{
    void Console::Show()
    {
        AllocConsole();

        HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        int crt = _open_osfhandle(reinterpret_cast<long&>(outputHandle), _O_TEXT);
        FILE* fOut = _fdopen(crt, "w");
        setvbuf(fOut, NULL, _IONBF, 1);
        *stdout = *fOut;

        HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
        crt = _open_osfhandle(reinterpret_cast<long&>(inputHandle), _O_TEXT);
        FILE* fIn = _fdopen(crt, "r");
        setvbuf(fIn, NULL, _IONBF, 128);
        *stdin = *fIn;

        std::ios::sync_with_stdio();
    }
}

#endif
