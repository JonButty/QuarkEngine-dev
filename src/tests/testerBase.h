/********************************************************************
	created:	2015/01/27
	filename: 	testerBase.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef TESTERBASE_H
#define TESTERBASE_H

#include "testCodes.h"

class TesterBase
{
public:
    TesterBase(TestOutputLevel level);
    virtual unsigned int Test() = 0;
protected:
    virtual bool load_() = 0;
    virtual bool unload_() = 0;
protected:
    TestOutputLevel level_;
};

#endif