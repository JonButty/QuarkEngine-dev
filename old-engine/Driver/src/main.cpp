//#include <vld.h>
#include "configBuild.h"
#include "main.h"
#include "coreSystem.h"
#include <iostream>

// TODO
// Create a pre build process that parses an xml file that associates mesh files
// with mesh types to generate the meshtypes.h file
int WINAPI WinMain(HINSTANCE ,
                   HINSTANCE ,
                   PSTR ,
                   int )
{
    Core::System::Instance().Load();
    Core::System::Instance().Run();
    Core::System::Instance().Unload();
    return 0;
}
