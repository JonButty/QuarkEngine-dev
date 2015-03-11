/*************************************************************************/
/*!
\file		DataFileIOXML.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef SIMIAN_DATAFILEIOXML_H_
#define SIMIAN_DATAFILEIOXML_H_

#include "SimianPlatform.h"
#include "DataFileIO.h"

class TiXmlElement;
class TiXmlBase;

namespace Simian
{
    class FileSubNode;

    class SIMIAN_EXPORT DataFileIOXML : public DataFileIO
    {
    private:
        FileObjectSubNode* registerNodeList_(TiXmlElement* const pRoot);
        FileSubNode* deserialize_(TiXmlElement* const pRoot);
        void serialize_(TiXmlElement * linkTo, FileSubNode* linkFrom);
    public:
        bool Read(const DataLocation& data);
        void Write(const std::string& path);
        std::string AsString();
    };

#if SIMIAN_DEFAULT_DATAFORMAT == SIMIAN_DATAFORMAT_XML
    typedef DataFileIOXML DataFileIODefault;
#endif
}

#endif
