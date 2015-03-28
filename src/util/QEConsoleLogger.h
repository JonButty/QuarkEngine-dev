/*!*****************************************************************************

\class 	QEConsoleLogger
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QECONSOLELOGGER_H
#define QECONSOLELOGGER_H

#include "QEDefines.h"

#include "util/QELogger.h"

class QEConsoleLogger : public QELogger
{
public:

    QE_API QEConsoleLogger();
    QE_API ~QEConsoleLogger();

public:

    QE_API void Load();
    QE_API void Log(const std::string& str);

};

#endif