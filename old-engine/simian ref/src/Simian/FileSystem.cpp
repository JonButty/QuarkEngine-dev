/*****************************************************************************/
/*!
\file		FileSystem.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/FileSystem.h"
#include "Simian/Debug.h"
#include "Simian/OS.h"

#include <fstream>

namespace Simian
{
    void FileSystem::MemoryFromFilePath(const std::string& filePath, s8*& buffer, size_t& size)
    {
        std::ifstream file;
        file.open(filePath, std::ios::binary);
        SAssert(!file.fail(), "Failed to open file.");
        file.seekg(0, std::ios::end);
        size = (size_t)file.tellg();
        file.seekg(0, std::ios::beg);
        buffer = new s8[size];
        file.read(buffer, size);
        file.close();
    }

    std::string FileSystem::GetDocumentsFolder()
    {
        return OS::Instance().GetSpecialFolderPath(OS::SF_DOCUMENTS);
    }

    std::string FileSystem::GetUserFolder()
    {
        return OS::Instance().GetSpecialFolderPath(OS::SF_USER);
    }
}
