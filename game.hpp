#ifndef GAME_HPP
#define GAME_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
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

class person;
class colony;
class gameWindow;
class map;
class game {
	std::vector<std::shared_ptr<buildingType>> buildingTypes;
	std::vector<std::shared_ptr<attackType>> attackTypes;
	std::vector<std::shared_ptr<unitType>> unitTypes;

	std::vector<std::shared_ptr<person>> people;
	std::vector<std::shared_ptr<colony>> colonies;
	std::vector<std::shared_ptr<map>> maps;

	std::shared_ptr<gameWindow> win;

	public:
		game();

		void Begin();
		screentype_t ThrowToColonyScreen(std::shared_ptr<colony> col);
		screentype_t ThrowToMapScreen(std::shared_ptr<map> theMap,
				int centerColm, int centerRow);

		bool Tick();	// false means quit, true means continue ticking

		std::vector<std::string> LoadDefFile(const std::string& name);
		void ReadAttackTypes(); // read exported file listing attack types
		void ReadUnitTypes(); // read exported file listing unit specs
		void ReadBuildingTypes(); // read exported file listing building types
		std::vector<std::shared_ptr<buildingType>> BuildingTypes();

		std::shared_ptr<person> CreatePerson(const std::shared_ptr<tile> loc,
				const std::shared_ptr<unitType> spec, const char faction);
		std::shared_ptr<colony> CreateColony(std::shared_ptr<map> parentMap,
				const int row, const int colm, const int faction);
		std::shared_ptr<map> CreateMap();

		std::shared_ptr<gameWindow> Window() { return win; }

		void NextTurn();
};

#endif
