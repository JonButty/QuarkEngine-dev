/*!*****************************************************************************

\file 	QEGlobalNode.h
\date 	2015/03/27
\author	Jonathan Butt
\brief	

*******************************************************************************/
#ifndef QEGLOBALNODE_H
#define QEGLOBALNODE_H

template <typename T>
class QEGlobalNode
{
public:

    QEGlobalNode()
    {
        next_ = Head();
        Head() = reinterpret_cast<T*>(this);
    }

public:

    T* Next() const
    {
        return next_;
    }

    static T*& Head()
    {
        static T* head = 0;
        return head;
    }

private:

    T* next_;

};

#endif