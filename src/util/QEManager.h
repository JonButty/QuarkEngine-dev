/*!*****************************************************************************

\class 	QEManager
\date 	2015/03/28
\author	Jonathan Butt
\brief	
\todo automatically register itself to system loader

*******************************************************************************/
#ifndef QEMANAGER_H
#define QEMANAGER_H

#include "common/QEDefines.h"
#include "common/QETypes.h"
#include "QESingleton.h"

#include <string>

template <typename T>
class QEManager : public QESingleton<T>
{
public:

    QE_API QEManager()
    {
    }

    QE_API virtual ~QEManager()
    {
    }

public:

    QE_API virtual int Load()
    {
        return 0;
    }

    QE_API virtual int Initialize()
    {
        return 0;
    }

    QE_API virtual int Update()
    {
        return 0;
    }

    QE_API virtual int Deinitialize()
    {
        return 0;
    }

    QE_API virtual int Unload()
    {
        return 0;
    }
};

#endif