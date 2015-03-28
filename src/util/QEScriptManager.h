/*!*****************************************************************************

\class 	QEScriptManager
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QESCRIPTMANAGER_H
#define QESCRIPTMANAGER_H

#include "QESingleton.h"
#include "QEManager.h"

class QEScriptManager : public QESingleton<QEScriptManager>
{
public:
    QEScriptManager();
    virtual ~QEScriptManager();
};

#endif
