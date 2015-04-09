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
    
    QE_API QELogger();
    QE_API virtual ~QELogger();

public:

    QE_API virtual void Load() ;
    QE_API virtual void Unload();
    QE_API virtual void Log(const std::string& str) = 0;

};

#endif