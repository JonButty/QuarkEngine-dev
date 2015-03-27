#include "mathTest.h"
#include "util/luaTest.h"
#include "util/QELog.h"

#include <iostream>
#include <fstream>

// Allow creation of test packages via script or testing components by loading
// test packages

// TODO integrate scripting language that allows for setting up an environment,
// and calling functions with certain arguments
// Print out of results should be automatic
// TODO Diff files
int main(int argc, char ** argv)
{
    QELogManager::InstancePtr()->Load();
    QELOG_V("test log");

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
    LuaTest::Test();

    // QETest [Test Name]
    //if(argc == 2)
    //{
    //    std::ifstream file(argv)
    //}
    //MathTest::Vector2Test<int>();
    return 0;
}