#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <memory>
#include <iostream>

#include "templates.hpp"
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

	std::vector<std::shared_ptr<person>> people;
	std::vector<std::shared_ptr<colony>> colonies;
	std::vector<std::shared_ptr<map>> maps;

	std::shared_ptr<gameWindow> win;

	public:
		game();
		bool Tick();	// false means quit, true means continue ticking

		void ReadBuildingTypes(); // read exported file listing building types
		std::vector<std::shared_ptr<buildingType>> BuildingTypes();

		std::shared_ptr<person> CreatePerson(const int x, const int y);
		std::shared_ptr<colony> CreateColony(std::shared_ptr<map> parentMap,
				const int row, const int colm);
		std::shared_ptr<map> CreateMap();

		std::shared_ptr<gameWindow> Window() { return win; }

		void NextTurn();
};

#endif
