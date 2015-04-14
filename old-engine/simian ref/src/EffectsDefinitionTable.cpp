#include "Simian/Debug.h"

#include "EffectsDefinitionTable.h"

namespace Descension
{
	const Simian::s32& EffectsDefinitionTable::Key(const std::string& key) const
	{
		std::map<std::string, Simian::s32>::const_iterator it = idTables_.find(key);

		SAssert(it != idTables_.end(),"The name does not exist in the definition table.");
		return it->second;
	}
}