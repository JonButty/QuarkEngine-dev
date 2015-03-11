/*****************************************************************************/
/*!
\file		DefinitionTable.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	March 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_DEFINITIONTABLE_H_
#define SIMIAN_DEFINITIONTABLE_H_

#include "SimianPlatform.h"
#include "DataLocation.h"
#include <map>
#include <string>
#include <vector>

namespace Simian
{
    class SIMIAN_EXPORT DefinitionTable
    {
    private:
        std::map<std::string, s32> idTables_;
    public:
        const std::string& Key(s32 value) const;
        const s32 Value(const std::string& enumName) const;
        bool HasValue(const std::string& enumName) const;
    private:
        void SetMemberName(std::string& currName, std::string& prevName);
        bool InCommentState(const s8* currChar, s8* tempCommC, s32* commState);
    public:
        void Load(const std::vector<DataLocation>& filePathVec);
    };
}

#endif
