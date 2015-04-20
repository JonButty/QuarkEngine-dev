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
    void CreateLuaFile();
protected:

    QEScriptObject* scriptObj_;

};

#endif