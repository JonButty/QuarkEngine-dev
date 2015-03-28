/*!*****************************************************************************

\class 	QEScriptObject
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
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