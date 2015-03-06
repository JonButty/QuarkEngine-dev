/********************************************************************
	created:	2015/01/27
	filename: 	testCodes.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef TESTCODES_H
#define TESTCODES_H

enum TestReturnCode
{
    TRC_OK,
    TRC_WARNING,
    TRC_ERROR,
    TRC_TOTAL
};

enum TestOutputLevel
{
    TOL_VERBOSE,
    TOL_WARNINGs,
    TOL_ERRORS,
    TOL_TOTAL
};

#endif