/*!*****************************************************************************

\class 	QEBootstraper
\date 	2015/05/22
\author	Jonathan Butt
\brief	

*******************************************************************************/

#ifndef QEBOOTSTRAPER_H
#define QEBOOTSTRAPER_H

#include "util/QESingleton.h"
#include "common/QETypes.h"
#include "common/QEDefines.h"

#include <string>
#include <map>

class QEModule;

class QEBootstrapper : public QESingleton<QEBootstrapper>
{
public:
    
    QEBootstrapper();
    ~QEBootstrapper();

public:

    QE_BOOL Load(QE_IN QE_INT argc,
                 QE_IN QE_CHAR ** argv);
};

#endif