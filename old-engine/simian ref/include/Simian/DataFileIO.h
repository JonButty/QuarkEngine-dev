/*************************************************************************/
/*!
\file		DataFileIO.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_DATAFILEIO_H_
#define SIMIAN_DATAFILEIO_H_

#include "SimianPlatform.h"
#include "DataLocation.h"

#include "FileObjectSubNode.h"
#include "FileArraySubNode.h"
#include "FileDataSubNode.h"

namespace Simian
{
    class FileObjectSubNode;
    typedef FileObjectSubNode FileRootSubNode;

    class SIMIAN_EXPORT DataFileIO
    {
    public:
        enum StatusFlags
        {
            SF_OPENED
        };
    private:
        FileRootSubNode* root_;
        u8 flags_;
    public:
        FileRootSubNode* Root() const;
        void Root(FileRootSubNode* val);

        u8 Flags() const;
        void Flags(u8 val);
    private:
        DataFileIO(const DataFileIO&);
        DataFileIO& operator=(const DataFileIO&);
    public:
        DataFileIO();
        virtual ~DataFileIO();

        S_NIMPL virtual bool Read(const DataLocation& data) = 0;
        S_NIMPL virtual void Write(const std::string& path) = 0;
        S_NIMPL virtual std::string AsString() = 0;
        void Close();
    };
}

#if SIMIAN_DEFAULT_DATAFORMAT == SIMIAN_DATAFORMAT_JSON
#include "Simian/DataFileIOJson.h"
#elif SIMIAN_DEFAULT_DATAFORMAT == SIMIAN_DATAFORMAT_XML
#include "Simian/DataFileIOXML.h"
#endif

#endif
