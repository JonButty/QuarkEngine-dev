/*!*****************************************************************************

\class 	modules
\date 	2015/05/22
\author	Jonathan Butt
\brief	Add appropriate code to declare modules. Currently not order specific

*******************************************************************************/

#ifndef MODULES_H
#define MODULES_H

#include "util/QEModule.h"
#include "util/QELogManager.h"
#include "util/QEScriptManager.h"

QE_MODULES_BEGIN
QE_DECLARE_MODULE(QELogManager)
QE_DECLARE_MODULE(QEScriptManager)
QE_MODULES_END

#endif