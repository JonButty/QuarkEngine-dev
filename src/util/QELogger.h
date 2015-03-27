/********************************************************************
	created:	2015/03/19
	filename: 	QELogger.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef QELOGGER_H
#define QELOGGER_H

#include "QEDefines.h"

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