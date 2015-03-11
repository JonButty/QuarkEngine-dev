#ifndef FILEIO_H
#define FILEIO_H

#include <fstream>

namespace Util
{
    struct FileHandle
    {
        std::ifstream iStream_;
        std::ofstream oStream_;
    };
}

#endif