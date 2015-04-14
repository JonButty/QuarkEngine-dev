/*!*****************************************************************************

\class 	QEFileSystem
\date 	2015/03/28
\author	Jonathan Butt
\brief	

*******************************************************************************/
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