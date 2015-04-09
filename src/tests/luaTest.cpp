#include "stdafx.h"
#include "luaTest.h"

#define TEST_FILE "data/test/luaTest.lua"

QE_BOOL LuaTest::Test()
{
    QEScriptObject* scriptObj = QEScriptManager::InstancePtr()->LoadScript("data/config/config.lua");
    QE_BOOL boolVal = true;
    QEScriptManager::InstancePtr()->GetBool(scriptObj,"testBool",&boolVal);
    QE_LOGV("TestBool is " << boolVal);

    QE_INT intVal;
    QEScriptManager::InstancePtr()->GetInt(scriptObj,"number",&intVal);
    QE_LOGV("Number is" << intVal);
    /*lua_State* L = luaL_newstate();
    int i = luaL_dofile(L, "data/config/config.lua");
    luaL_openlibs(L);
    lua_pcall(L, 0, 0, 0);
    LuaRef s = getGlobal(L, "testString");
    LuaRef n = getGlobal(L, "number");
    std::string luaString = s.cast<std::string>();
    int answer = n.cast<int>();
    std::cout << luaString << std::endl;
    std::cout << "And here's our number:" << answer << std::endl;
    */
}

/*!*****************************************************************************

\name   testGetBool_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testGetBool_()
{
    QEScriptObject* scriptObj = QEScriptManager::InstancePtr()->LoadScript(TEST_FILE);
}

/*!*****************************************************************************

\name   testGetInt_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testGetInt_()
{

}

/*!*****************************************************************************

\name   testGetFloat_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testGetFloat_()
{

}

/*!*****************************************************************************

\name   testGetString_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testGetString_()
{

}

/*!*****************************************************************************

\name   testGetInvalidNameBool_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testGetInvalidNameBool_()
{

}

/*!*****************************************************************************

\name   testGetInvalidNameInt_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testGetInvalidNameInt_()
{

}

/*!*****************************************************************************

\name   testGetInvalidNameFloat_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testGetInvalidNameFloat_()
{

}

/*!*****************************************************************************

\name   testGetInvalidNameString_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testGetInvalidNameString_()
{

}

/*!*****************************************************************************

\name   testGetInvalidTypeBool_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testGetInvalidTypeBool_()
{

}

/*!*****************************************************************************

\name   testGetInvalidTypeInt_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testGetInvalidTypeInt_()
{

}

/*!*****************************************************************************

\name   testGetInvalidTypeFloat_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testGetInvalidTypeFloat_()
{

}

/*!*****************************************************************************

\name   testGetInvalidTypeString_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testGetInvalidTypeString_()
{

}

/*!*****************************************************************************

\name   testLoadFile_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testLoadFile_()
{
    QEScriptObject* scriptObj = QEScriptManager::InstancePtr()->LoadScript(TEST_FILE);
    return QEScriptManager::InstancePtr()->ErrorCheck
}

/*!*****************************************************************************

\name   testLoadInvalidFile_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testLoadInvalidFile_()
{

}

/*!*****************************************************************************

\name   testReloadInvalidFile_
\return QE_BOOL

\brief

*******************************************************************************/
QE_BOOL LuaTest::testReloadInvalidFile_()
{

}
