#ifndef LUATEST_H
#define LUATEST_H

#include "stdafx.h"
#include "gtest/gtest.h"

class LuaTest : public ::testing::Test
{
protected:

    LuaTest();
    virtual ~LuaTest();
    
protected:

    void SetUp();
    void TearDown();
    /*
private:
    QE_BOOL testGetBool_();
    QE_BOOL testGetInt_();
    QE_BOOL testGetFloat_();
    QE_BOOL testGetString_();
    QE_BOOL testGetInvalidNameBool_();
    QE_BOOL testGetInvalidNameInt_();
    QE_BOOL testGetInvalidNameFloat_();
    QE_BOOL testGetInvalidNameString_();
    QE_BOOL testGetInvalidTypeBool_();
    QE_BOOL testGetInvalidTypeInt_();
    QE_BOOL testGetInvalidTypeFloat_();
    QE_BOOL testGetInvalidTypeString_();
    QE_BOOL testLoadFile_();
    QE_BOOL testLoadInvalidFile_();
    QE_BOOL testReloadInvalidFile_();
    QE_BOOL testMultipleReferences_();
    */
};

#endif