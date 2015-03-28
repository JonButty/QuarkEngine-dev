#include "common/stdafx.h"
#include "mathTest.h"
#include "luaTest.h"

#include <iostream>
#include <fstream>

// Allow creation of test packages via script or testing components by loading
// test packages

// \todo integrate scripting language that allows for setting up an environment,
// and calling functions with certain arguments
// Print out of results should be automatic
// \todo Diff files to test testing output
int main(int argc, char ** argv)
{
    QELogManager::InstancePtr()->Load();
    std::vector<std::string> filter;
    filter.push_back("main.cpp");

    QELogManager::InstancePtr()->SetFilter(&filter);
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
    //LuaTest::Test();

    // QETest [Test Name]
    //if(argc == 2)
    //{
    //    std::ifstream file(argv)
    //}
    //MathTest::Vector2Test<int>();
    return 0;
}