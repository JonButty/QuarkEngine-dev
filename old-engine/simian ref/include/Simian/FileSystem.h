/*****************************************************************************/
/*!
\file		FileSystem.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_FILESYSTEM_H_
#define SIMIAN_FILESYSTEM_H_

#include "SimianPlatform.h"
#include <string>

namespace Simian
{
    /** Performs filesystem related operations. */
    class SIMIAN_EXPORT FileSystem
    {
    public:
        /** Loads a file into a buffer in memory.
            @remarks
                The buffer is not automatically deallocated. Use the delete 
                keyword to deallocate buffers produced by MemoryFromFilePath.
        */
        static void MemoryFromFilePath(const std::string& filePath, s8*& buffer, size_t& size);

        /** Gets the "My Documents" folder of the system. */
        static std::string GetDocumentsFolder();

        /** Get the user's home folder. */
        static std::string GetUserFolder();
    };
}

#endif
