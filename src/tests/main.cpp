#include "stdafx.h"
#include "mathTest.h"
#include "luaTest.h"

#include <iostream>
#include <fstream>

// Allow creation of test packages via script or testing components by loading
// test packages

/*! 
\todo integrate scripting language that allows for setting up an environment,
      and calling functions with certain arguments
\todo Diff files to test testing output
\todo Component to initialize and unload manager objects
*/
int main(int argc, char ** argv)
{
    QELogManager::InstancePtr()->Load();
    QEScriptManager::InstancePtr()->Load();
    
    ::testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();

    /*
    if(argc == 1)
    {
        std::cout << "Creating a test package:" << std::endl;
        std::cout << "QETest [Test Script]" << std::endl << std::endl;
        std::cout << "Running a test package:" << std::endl;
        std::cout << "QETest [Test Package] [Test Results*]" << std::endl << std::endl;
        std::cout << "* - Optional argument" << std::endl;
        return 0;
    }
    */

    // QETest [Test Name]
    //if(argc == 2)
    //{
    //    std::ifstream file(argv)
    //}
    //MathTest::Vector2Test<int>();
    return 0;
}