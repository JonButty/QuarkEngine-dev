/************************************************************************/
/*!
\file		DataLocation.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/DataLocation.h"
#include "Simian/Debug.h"

#include <fstream>

namespace Simian
{
    DataLocation::DataLocation()
        : buffer_(0),
          size_(0),
          invalid_(true),
          managed_(false)
    {
    }

    DataLocation::DataLocation( const s8* filePath )
        : identifier_(filePath),
          buffer_(0),
          size_(0),
          invalid_(true),
          managed_(true)
    {
        loadFile_(filePath);
    }

    DataLocation::DataLocation( const std::string& filePath )
        : identifier_(filePath),
          buffer_(0),
          size_(0),
          invalid_(true),
          managed_(true)
    {
        loadFile_(filePath);
    }

    DataLocation::DataLocation( s8* buffer, u32 size, const std::string& identifier, bool managed)
        : identifier_(identifier),
          buffer_(buffer),
          size_(size),
          invalid_(buffer && size ? false : true),
          managed_(managed)
    {
    }

    DataLocation::DataLocation( const DataLocation& copy )
        : identifier_(copy.identifier_),
          buffer_(copy.invalid_ ? 0 : new s8[copy.size_]),
          size_(copy.invalid_ ? 0 : copy.size_),
          invalid_(copy.invalid_),
          managed_(true)
    {
        if (!invalid_)
            memcpy(buffer_, copy.buffer_, sizeof(s8) * size_);
    }

    DataLocation::~DataLocation()
    {
        if (managed_)
            delete [] buffer_;
    }

    //--------------------------------------------------------------------------

    const std::string& DataLocation::Identifier() const
    {
        return identifier_;
    }

    s8* DataLocation::Memory() const
    {
        SAssert(!invalid_, "Data location is invalid.");
        return buffer_;
    }

    u32 DataLocation::Size() const
    {
        SAssert(!invalid_, "Data location is invalid.");
        return size_;
    }

    bool DataLocation::Invalid() const
    {
        return invalid_;
    }

    bool DataLocation::Managed() const
    {
        return managed_;
    }

    //--------------------------------------------------------------------------

    DataLocation::operator bool() const
    {
        return !Invalid();
    }

    DataLocation& DataLocation::operator=( const DataLocation& copy )
    {
        if (&copy == this)
            return *this;

        identifier_ = copy.identifier_;
        invalid_ = copy.invalid_;

        if (!invalid_)
        {
            delete [] buffer_;
            buffer_ = new s8[copy.size_];
            memcpy(buffer_, copy.buffer_, sizeof(s8) * copy.size_);
            size_ = copy.size_;
            managed_ = true;
        }
        
        return *this;
    }

    //--------------------------------------------------------------------------

    void DataLocation::loadFile_(const std::string& filePath)
    {
        std::ifstream file(filePath, std::ios::binary);
        if (file)
        {
            file.seekg(0, std::ios::end);
            size_ = static_cast<u32>(file.tellg());
            file.seekg(0, std::ios::beg);
            buffer_ = new s8[size_];
            file.read(buffer_, size_);
            invalid_ = false;
        }
    }
}
