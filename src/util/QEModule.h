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
#include "core/QEModuleManager.h"

#include <string>

class QEModule
{
public:

    QEModule()
    {
    }

    virtual ~QEModule()
    {
    }

public:

    virtual int Load()
    {
        return 0;
    }

    virtual int Initialize()
    {
        return 0;
    }

    virtual int Update()
    {
        return 0;
    }

    virtual int Deinitialize()
    {
        return 0;
    }

    virtual int Unload()
    {
        return 0;
    }
};

#endif