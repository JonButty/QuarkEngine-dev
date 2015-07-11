/*!*****************************************************************************

\class 	QELog
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QELOG_H
#define QELOG_H

#include "util/QELogManager.h"
#include "common/QEDefines.h"

#include <sstream>
#include <string>

#define QE_LOGV(msg)\
{\
    std::stringstream ss;\
    ss << msg << '\0';\
    QE_MODULE_PTR(QELogManager)->Log(__FILE__,__LINE__,ss.str(),QELogManager::L_VERBOSE);\
}

#define QE_LOGW(msg)\
{\
    std::stringstream ss;\
    ss << msg << '\0';\
    QE_MODULE_PTR(QELogManager)->Log(__FILE__,__LINE__,ss.str(),QELogManager::L_WARNING);\
}

#define QE_LOGE(msg)\
{\
    std::stringstream ss;\
    ss << msg << '\0';\
    QE_MODULE_PTR(QELogManager)->Log(__FILE__,__LINE__,ss.str(),QELogManager::L_ERROR);\
}

#endif