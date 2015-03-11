#ifndef FILEINPUT_H
#define FILEINPUT_H

#include "configUtilitiesDLL.h"
#include "delegate.h"
#include "fileHandle.h"

#include <string>

namespace Util
{
    struct CharDelegateParam
        :   public DelegateParameter
    {
        char char_;
    };

    struct StringDelegateParam
        :   public DelegateParameter
    {
        std::string string_;
    };

    class ENGINE_UTILITIES_EXPORT LinePredicate
    {
    public:
        virtual ~LinePredicate(){}
        virtual bool operator()(const std::string&) const = 0;
    };

    class ENGINE_UTILITIES_EXPORT FileInput
    {
    public:
        static void Open(FileHandle& handle, const std::string& filePath);
        static void Close(FileHandle& handle);
        static void Parse(FileHandle& handle,
                          LinePredicate* format,
                          Util::Delegate callback);
    };
}

#endif