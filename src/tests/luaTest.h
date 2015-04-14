#ifndef LUATEST_H
#define LUATEST_H

#include "stdafx.h"

class LuaTest
{
public:
    static QE_BOOL Test();
private:
    static QE_BOOL testGetBool_();
    static QE_BOOL testGetInt_();
    static QE_BOOL testGetFloat_();
    static QE_BOOL testGetString_();
    static QE_BOOL testGetInvalidNameBool_();
    static QE_BOOL testGetInvalidNameInt_();
    static QE_BOOL testGetInvalidNameFloat_();
    static QE_BOOL testGetInvalidNameString_();
    static QE_BOOL testGetInvalidTypeBool_();
    static QE_BOOL testGetInvalidTypeInt_();
    static QE_BOOL testGetInvalidTypeFloat_();
    static QE_BOOL testGetInvalidTypeString_();
    static QE_BOOL testLoadFile_();
    static QE_BOOL testLoadInvalidFile_();
    static QE_BOOL testReloadInvalidFile_();
};

#endif