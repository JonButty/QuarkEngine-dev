#ifndef DESCENSION_EFFECTSDEFINITIONTABLE_H_
#define DESCENSION_EFFECTSDEFINITIONTABLE_H_

#include "Simian\SimianPlatform.h"
#include "Simian\SimianTypes.h"
#include "Simian\DataLocation.h"
#include <map>
#include <string>
#include <vector>

namespace Descension
{
    class EffectsDefinitionTable
    {
    private:
        std::map<std::string, Simian::s32> idTables_;
    public:
        const Simian::s32& Key(const std::string& key) const;
        const std::string& Value(const Simian::s32 val) const;
        bool HasValue(const std::string& enumName) const;
    private:
        void setMemberName_(std::string& currName, std::string& prevName);
        bool inCommentState_(const Simian::s8* currChar, Simian::s8* tempCommC, Simian::s32* commState);
    public:
        void Load(const std::vector<Simian::DataLocation>& filePathVec);
    };
}

#endif
