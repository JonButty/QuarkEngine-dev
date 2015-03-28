/*!*****************************************************************************

\file 	QEGlobalNode.h
\date 	2015/03/27
\author	Jonathan Butt
\brief	

*******************************************************************************/

#ifndef QEGLOBALNODE_H
#define QEGLOBALNODE_H

#include "common/QEDefines.h"

template <typename T>
class QEGlobalNode
{
public:

    QE_API QEGlobalNode()
    {
        next_ = Head();
        Head() = reinterpret_cast<T*>(this);
    }

public:

    QE_API T* Next() const
    {
        return next_;
    }

    QE_API static T*& Head()
    {
        static T* head = 0;
        return head;
    }

private:

    T* next_;
};

#endif