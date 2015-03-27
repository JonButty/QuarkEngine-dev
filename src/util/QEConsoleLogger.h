/********************************************************************
	created:	2015/03/19
	filename: 	QEConsoleLogger.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

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