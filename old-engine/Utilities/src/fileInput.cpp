#include "fileInput.h"
#include "utilLogger.h"

#include <iostream>

namespace Util
{
    void FileInput::Open( FileHandle& handle,
                          const std::string& filePath )
    {
        handle.iStream_.open(filePath.c_str());
        if(!handle.iStream_.is_open())
        {
            UTIL_LOG(filePath << " failed to load.");
        }
    }

    void FileInput::Close( FileHandle& handle )
    {
        handle.iStream_.close();
    }

    void FileInput::Parse( FileHandle& handle,
                           LinePredicate* format,
                           Util::Delegate callback )
    {
        // Iterate until correct section
        while(!handle.iStream_.eof())
        {
            std::string in;
            getline(handle.iStream_,in);

            if((*format)(in))
            {
                StringDelegateParam param;
                param.string_ = in;
                callback(param);
                break;
            }
        }

        // Iterate until section over
        while(!handle.iStream_.eof())
        {
            std::string in;
            getline(handle.iStream_,in);
            
            if(!(*format)(in))
                break;

            StringDelegateParam param;
            param.string_ = in;
            callback(param);
        }
        handle.iStream_.clear();
        handle.iStream_.seekg(0,std::ios_base::beg);
    }
}