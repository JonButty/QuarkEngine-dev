#include "stdafx.h"
#include "luaTest.h"

#include <fstream>

#define TEST_FILEPATH   "data/test/luaTest.lua"
#define TEST_STRING_VAR "testString"
#define TEST_STRING_VAL "LuaBridge works!"
#define TEST_INT_VAR    "testNumber"
#define TEST_INT_VAL    42
#define TEST_BOOL_VAR   "testBool"
#define TEST_BOOL_VAL   true

//QE_BOOL LuaTest::Test()
//{
//    QE_BOOL result = true;
//    result = testLoadFile_() ? result : false;
//    return result;
//}

/*!*****************************************************************************

\name   testGetBool_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testGetBool_()
//{
//    //QEScriptObject* scriptObj = QEScriptManager::InstancePtr()->LoadScript(TEST_FILE);
//    return true;
//}

/*!*****************************************************************************

\name   testGetInt_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testGetInt_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   testGetFloat_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testGetFloat_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   testGetString_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testGetString_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   testGetInvalidNameBool_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testGetInvalidNameBool_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   testGetInvalidNameInt_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testGetInvalidNameInt_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   testGetInvalidNameFloat_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testGetInvalidNameFloat_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   testGetInvalidNameString_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testGetInvalidNameString_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   testGetInvalidTypeBool_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testGetInvalidTypeBool_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   testGetInvalidTypeInt_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testGetInvalidTypeInt_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   testGetInvalidTypeFloat_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testGetInvalidTypeFloat_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   testGetInvalidTypeString_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testGetInvalidTypeString_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   testLoadFile_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testLoadFile_()
//{
//    QEScriptObject* scriptObj;
//    if(!QEScriptManager::InstancePtr()->LoadScript(scriptObj,TEST_FILE))
//        return false;
//    QEScriptManager::InstancePtr()->UnloadScript(scriptObj);
//    return true;
//}

/*!*****************************************************************************

\name   testLoadInvalidFile_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testLoadInvalidFile_()
//{
//    QEScriptObject* scriptObj;
//    if(QEScriptManager::InstancePtr()->LoadScript(scriptObj,"foo.lua"))
//        return false;
//    return true;
//}

/*!*****************************************************************************

\name   testReloadInvalidFile_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testReloadInvalidFile_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   testMultipleReferences_
\return QE_BOOL

\brief

*******************************************************************************/
//QE_BOOL LuaTest::testMultipleReferences_()
//{
//    return true;
//}

/*!*****************************************************************************

\name   LuaTest
\return

\brief

*******************************************************************************/
LuaTest::LuaTest()
{
    //QEScriptManager::InstancePtr()->Load();
    CreateLuaFile();
}

/*!*****************************************************************************

\name   ~LuaTest
\return

\brief

*******************************************************************************/
LuaTest::~LuaTest()
{
    //QEScriptManager::InstancePtr()->Unload();
}

/*!*****************************************************************************

\name   SetUp
\return void

\brief

*******************************************************************************/
void LuaTest::SetUp()
{
    scriptObj_ = 0;
}

/*!*****************************************************************************

\name   TearDown
\return void

\brief

*******************************************************************************/
void LuaTest::TearDown()
{
    //QEScriptManager::InstancePtr()->UnloadScript(scriptObj_);
}

/*!*****************************************************************************

\name   CreateLuaFile
\return void

\brief

*******************************************************************************/
void LuaTest::CreateLuaFile()
{
    std::ofstream file(TEST_FILEPATH);

    file << TEST_STRING_VAR << "=\"" << TEST_STRING_VAL << "\"\n";
    file << TEST_INT_VAR    << "="   << TEST_INT_VAL    << "\n";
    file << TEST_BOOL_VAR   << "="   << TEST_BOOL_VAL   << "\n";

    file.close();
}/*

TEST_F(LuaTest,LoadScriptFile)
{
    //EXPECT_TRUE(QEScriptManager::InstancePtr()->LoadScript(scriptObj_,TEST_FILEPATH));
}

TEST_F(LuaTest,LoadInvalidScriptFile)
{
    //EXPECT_FALSE(QEScriptManager::InstancePtr()->LoadScript(scriptObj_,"foo.lua"));
}

TEST_F(LuaTest,GetTypeBool)
{ 
    //EXPECT_TRUE(QEScriptManager::InstancePtr()->LoadScript(scriptObj_,TEST_FILEPATH));
    
    bool val = false;
    //QEScriptManager::InstancePtr()->GetBool(scriptObj_,TEST_BOOL_VAR,&val);
    
    EXPECT_EQ(TEST_BOOL_VAL,val);
}*/