/********************************************************************
	created:	2015/01/27
	filename: 	QEFileSystem.h
	author:		Jonathan Butt
	purpose:	An interface that allows users to manipulate files without
                needing to know the actual path.
*********************************************************************/

#ifndef QEFILESYSTEM_H
#define QEFILESYSTEM_H

#include "QESingleton.h"

class QEFileSystem : public QESingleton<QEFileSystem>
{
public:
    QEFileSystem();
private:
    void generateTree_();
};

#endif