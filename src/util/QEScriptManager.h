/********************************************************************
	created:	2015/03/19
	filename: 	QEScriptManager.h
	author:		Jonathan Butt
	purpose:	
*********************************************************************/

#ifndef QESCRIPTMANAGER_H
#define QESCRIPTMANAGER_H

#include "QESingleton.h"

class QEScriptManager : public QESingleton<QEScriptManager>
{
public:
    QEScriptManager();
    virtual ~QEScriptManager();
};

#endif
