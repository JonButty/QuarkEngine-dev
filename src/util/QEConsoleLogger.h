/*!*****************************************************************************

\class 	QEConsoleLogger
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QECONSOLELOGGER_H
#define QECONSOLELOGGER_H

#include "common/QEDefines.h"
#include "util/QELogger.h"

class QEConsoleLogger : public QELogger
{
public:

    QEConsoleLogger();
    ~QEConsoleLogger();

public:

    void Load();
    void Log(const std::string& str);

};

#endif