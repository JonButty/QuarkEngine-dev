/*************************************************************************/
/*!
\file		DataFileIOJson.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_DATAFILEIOJSON_H_
#define SIMIAN_DATAFILEIOJSON_H_

#include "SimianPlatform.h"
#include "DataFileIO.h"

namespace Json
{
    class Value;
}

namespace Simian
{
    class FileSubNode;

    class SIMIAN_EXPORT DataFileIOJson: public DataFileIO
    {
    private:
        FileSubNode* createNode_(Json::Value& value, const std::string& name );
        void createJsonNode_(Json::Value& value, FileSubNode* node);
    public:
        bool Read( const DataLocation& data );
        void Write( const std::string& path );
        std::string AsString();
    };

#if SIMIAN_DEFAULT_DATAFORMAT == SIMIAN_DATAFORMAT_JSON
    typedef DataFileIOJson DataFileIODefault;
#endif
}

#endif
