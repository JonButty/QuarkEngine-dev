/*!*****************************************************************************

\class 	QELogger
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QELOGGER_H
#define QELOGGER_H

#include "common/QEDefines.h"

#include <string>

class QELogger
{
public:
    
    QELogger();
    virtual ~QELogger();

public:

    virtual void Load() ;
    virtual void Unload();
    virtual void Log(const std::string& str) = 0;

};

#endif