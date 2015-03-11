/************************************************************************/
/*!
\file		DataLocation.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_DATALOCATION_H_
#define SIMIAN_DATALOCATION_H_

#include "SimianPlatform.h"

#include <string>

namespace Simian
{
    class SIMIAN_EXPORT DataLocation
    {
    private:
        std::string identifier_;
        s8* buffer_;
        u32 size_;
        bool invalid_;
        bool managed_;
    public:
        const std::string& Identifier() const;

        s8* Memory() const;

        u32 Size() const;

        bool Invalid() const;

        bool Managed() const;
    public:
        operator bool() const;
        DataLocation& operator=(const DataLocation& copy);
    private:
        void loadFile_(const std::string& filePath);
    public:
        DataLocation();
        DataLocation(const s8* filePath);
        DataLocation(const std::string& filePath);
        DataLocation(s8* buffer, u32 size, const std::string& identifier = "", bool managed = false);
        DataLocation(const DataLocation& copy);
        virtual ~DataLocation();
    };
}

#endif
