/*!*****************************************************************************

\class 	QELog
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QELOG_H
#define QELOG_H

#include "QELogManager.h"

#define QELOG_V(msg) QELogManager::InstancePtr()->Log(__FILE__,__LINE__,msg,QELogManager::L_VERBOSE)
#define QELOG_W(msg) QELogManager::InstancePtr()->Log(__FILE__,__LINE__,msg,QELogManager::L_WARNING)
#define QELOG_E(msg) QELogManager::InstancePtr()->Log(__FILE__,__LINE__,msg,QELogManager::L_ERROR)

#endif