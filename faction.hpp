#ifndef FACTION_HPP
#define FACTION_HPP

#include <vector>
#include <string>

#include "building.hpp"

namespace TerraNova {

class Faction {
	static std::vector<BuildingType*> defaultBuildingTypes;

	public:
		static int NumberOfFactions() { return 2; }
		static std::string GenerateColonyName(const int)
			{ return "Aurora"; }
		static const std::vector<BuildingType*>& BuildingTypes(const int)
			{ return defaultBuildingTypes; }
		static void SetDefaultBuildingTypes(std::vector<BuildingType*> types)
			{ defaultBuildingTypes = std::move(types); }
};

} // namespace TerraNova

#endif
