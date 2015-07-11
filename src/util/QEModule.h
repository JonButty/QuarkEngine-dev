/*!*****************************************************************************

\class 	QEModule
\date 	2015/03/28
\author	Jonathan Butt
\brief	
\todo automatically register itself to system loader

*******************************************************************************/
#ifndef QEMODULE_H
#define QEMODULE_H

#include "common/QEDefines.h"
#include "common/QETypes.h"
#include "util/QESingleton.h"

#include <string>

class QEModule : public QESingleton <QEModule>
{
public:

    QEModule()
    {
    }

    virtual ~QEModule()
    {
    }

public:

    virtual QE_INT Load()
    {
        return 0;
    }

    virtual QE_INT Initialize()
    {
        return 0;
    }

    virtual QE_INT Update()
    {
        return 0;
    }

    virtual QE_INT Deinitialize()
    {
        return 0;
    }

    virtual QE_INT Unload()
    {
        return 0;
    }
};

#endif