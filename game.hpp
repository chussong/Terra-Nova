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
#include "unit.hpp"
#include "building.hpp"
#include "colony.hpp"
#include "map.hpp"
#include "gamewindow.hpp"

class Game {
	std::vector<std::shared_ptr<BuildingType>> buildingTypes;
	std::vector<std::shared_ptr<AttackType>> attackTypes;
	std::vector<std::shared_ptr<UnitType>> unitTypes;

	std::vector<std::shared_ptr<TileType>> tileTypes;
	std::vector<std::shared_ptr<Map>> maps;
	std::vector<std::shared_ptr<Unit>> units;
	std::vector<std::shared_ptr<Colony>> colonies;

	std::shared_ptr<GameWindow> win;

	void ClearDeadUnits();

	void StartTurn();
	void EndTurn();
	std::unique_ptr<Button> EndTurnButton(SDL_Renderer* ren);

	public:
		Game();

		void Begin();
		screentype_t ThrowToColonyScreen(std::shared_ptr<Colony> col);
		screentype_t ThrowToMapScreen(std::shared_ptr<Map> theMap,
				int centerRow, int centerColm);

		bool Tick();	// false means quit, true means continue ticking

		std::vector<std::string> LoadDefFile(const std::string& name);
		void ReadAttackTypes(); // read exported file listing attack types
		void ReadUnitTypes();
		void ReadBuildingTypes();
		const std::vector<std::shared_ptr<BuildingType>>& BuildingTypes() const;

		void ReadTileTypes();
		void ReadMap(const std::string& MapName);
		std::vector<std::string> ExtractMapSection(
				const std::vector<std::string>& lines, const unsigned int i);
		std::string ParseDescLine(const std::string& line, const std::string& key);

		std::map<char, std::shared_ptr<TileType>> ParseTerrainDictionary(
				const std::vector<std::string> desc);
		std::vector<std::vector<std::shared_ptr<Tile>>> ParseMap(
				const std::map<char, std::shared_ptr<TileType>> TileDict, 
				std::vector<std::string> desc);

		void ParseFeatures(std::shared_ptr<Map> parentMap, 
				const std::vector<std::string> desc);
		std::shared_ptr<Colony> ParseColony(std::shared_ptr<Map> parentMap,
				const std::vector<std::string>& desc);

		void ParseUnits(std::shared_ptr<Map> parentMap,
				const std::vector<std::string>& desc);
		std::shared_ptr<Unit> ParseUniqueUnit(std::shared_ptr<Map> parentMap,
				const std::vector<std::string>& UnitDesc);
		std::vector<std::shared_ptr<Unit>> ParseGenericUnits(
				std::shared_ptr<Map> parentMap, 
				const std::shared_ptr<UnitType> genericType, 
				std::vector<std::string> UnitDesc);

		std::shared_ptr<Unit> CreatePerson(Map* theMap, const int row,
				const int colm,
				const std::shared_ptr<UnitType> spec, const char faction);
		std::shared_ptr<Colony> CreateColony(std::shared_ptr<Map> parentMap,
				const int row, const int colm, const std::string& name, 
				const int faction);
		std::shared_ptr<Map> CreateMap();

		std::shared_ptr<GameWindow> Window() { return win; }

		void NextTurn();
};

#endif
