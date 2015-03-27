/********************************************************************
	created:	2015/03/19
	filename: 	QEScriptObject.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef QESCRIPTOBJECT_H
#define QESCRIPTOBJECT_H

#include <string>

struct QEScriptObject
{
    enum Status
    {
        S_OK = 0,
        S_COMPILE_ERRORS = -1,
        S_TOTAL
    };
    int status_;
    std::string filePath_;
};

#endif