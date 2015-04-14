/*************************************************************************/
/*!
\file		main.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include <Windows.h>
//#include <vld.h>

#include "Simian/Game.h"
#include "EngineSandbox/StateSandbox.h"

#include "Console.h"
//
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
#if _DEBUG
    Descension::Console::Show();
#endif

    Simian::Game::Instance().GameTitle("Bryan's Engine Sandbox");
    Simian::Game::Instance().Size(Simian::Vector2(1024, 768));
    Simian::Game::Instance().Start(Sandbox::StateSandbox::InstancePtr());
    return 0;
}
