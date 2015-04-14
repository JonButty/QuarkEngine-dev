/*!*****************************************************************************

\class 	QEScriptObject
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QESCRIPTOBJECT_H
#define QESCRIPTOBJECT_H

#include "common/QETypes.h"

#include <string>

struct QEScriptObject
{
    enum Status
    {
        S_OK,
        S_FILE_OPEN_ERROR,
        S_COMPILE_ERRORS,
        S_VAR_NOT_FOUND,
        S_INVALID_VAR_TYPE,
        S_TOTAL
    };

    QE_INT status_;
    std::string filePath_;
};

#endif