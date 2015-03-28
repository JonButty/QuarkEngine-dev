/*!*****************************************************************************

\file 	QEManager.h
\date 	2015/03/27
\author	Jonathan Butt
\brief	

*******************************************************************************/

#ifndef QEMANAGER_H
#define QEMANAGER_H

#include "common/QEDefines.h"
#include "common/QETypes.h"
#include "QEGlobalNode.h"

#include <string>

template <typename T>
class QEManager : public QEGlobalNode<QEManager<T> >
{
public:

    QE_API QEManager()
    {
        instance_ = reinterpret_cast<T*>(Head());
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

    QE_API virtual bool ErrorCheck(QE_IN QE_INT code) const
    {
        QE_UNUSED(code)
        return 0;
    }

    QE_API virtual std::string GetErrorString(QE_IN QE_INT code) const
    {
        QE_UNUSED(code)
        return "";
    }

public:

    QE_API static T& Instance()
    {
        return *InstancePtr();
    }

    QE_API static T* InstancePtr()
    {
        if(!instance_)
        {
            instance_ = new T();
            atexit(QEManager<T>::destroy_);
        }
        return instance_;
    }

private:
    
    static void destroy_()
    {
        delete instance_;
    }

private:

    static T* instance_;

};

template <typename T>
T* QEManager<T>::instance_ = 0;

#endif