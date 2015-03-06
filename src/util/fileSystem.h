/********************************************************************
	created:	2015/01/27
	filename: 	fileSystem.h
	author:		Jonathan Butt
	purpose:	An interface that allows users to manipulate files without
                needing to know the actual path.
*********************************************************************/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "singleton.h"

namespace Util
{
    class FileSystem : public Singleton<FileSystem>
    {
    public:
        FileSystem();
    private:
        void generateTree_();
    };
}

#endif