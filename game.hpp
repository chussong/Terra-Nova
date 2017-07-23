#ifndef GAME_HPP
#define GAME_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include <fstream>
#include <functional>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "templates.hpp"
#include "exceptions.hpp"
#include "person.hpp"
#include "building.hpp"
#include "colony.hpp"
#include "building.hpp"
#include "map.hpp"
#include "gamewindow.hpp"
#include "ui.hpp"

class person;
class colony;
class gameWindow;
class map;
class game {
	std::vector<std::shared_ptr<buildingType>> buildingTypes;
	std::vector<std::shared_ptr<attackType>> attackTypes;
	std::vector<std::shared_ptr<unitType>> unitTypes;

	std::vector<std::shared_ptr<tileType>> tileTypes;
	std::vector<std::shared_ptr<map>> maps;
	std::vector<std::shared_ptr<person>> people;
	std::vector<std::shared_ptr<colony>> colonies;

	std::shared_ptr<gameWindow> win;

	void ClearDeadPeople();

	void StartTurn();
	void EndTurn();
	std::unique_ptr<Button> EndTurnButton(SDL_Renderer* ren);

	public:
		game();

		void Begin();
		screentype_t ThrowToColonyScreen(std::shared_ptr<colony> col);
		screentype_t ThrowToMapScreen(std::shared_ptr<map> theMap,
				int centerRow, int centerColm);

		bool Tick();	// false means quit, true means continue ticking

		std::vector<std::string> LoadDefFile(const std::string& name);
		void ReadAttackTypes(); // read exported file listing attack types
		void ReadUnitTypes();
		void ReadBuildingTypes();
		std::vector<std::shared_ptr<buildingType>> BuildingTypes();

		void ReadTileTypes();
		void ReadMap(const std::string& mapName);
		std::vector<std::string> ExtractMapSection(
				const std::vector<std::string>& lines, const unsigned int i);
		std::string ParseDescLine(const std::string& line, const std::string& key);

		std::map<char, std::shared_ptr<tileType>> ParseTerrainDictionary(
				const std::vector<std::string> desc);
		std::vector<std::vector<std::shared_ptr<tile>>> ParseMap(
				const std::map<char, std::shared_ptr<tileType>> tileDict, 
				std::vector<std::string> desc);

		void ParseFeatures(std::shared_ptr<map> parentMap, 
				const std::vector<std::string> desc);
		std::shared_ptr<colony> ParseColony(std::shared_ptr<map> parentMap,
				const std::vector<std::string>& desc);

		void ParseUnits(std::shared_ptr<map> parentMap,
				const std::vector<std::string>& desc);
		std::shared_ptr<person> ParseUniqueUnit(std::shared_ptr<map> parentMap,
				const std::vector<std::string>& unitDesc);
		std::vector<std::shared_ptr<person>> ParseGenericUnits(
				std::shared_ptr<map> parentMap, 
				const std::shared_ptr<unitType> genericType, 
				std::vector<std::string> unitDesc);

		std::shared_ptr<person> CreatePerson(map* theMap, const int row,
				const int colm,
				const std::shared_ptr<unitType> spec, const char faction);
		std::shared_ptr<colony> CreateColony(std::shared_ptr<map> parentMap,
				const int row, const int colm, const std::string& name, 
				const int faction);
		std::shared_ptr<map> CreateMap();

		std::shared_ptr<gameWindow> Window() { return win; }

		void NextTurn();
};

#endif
