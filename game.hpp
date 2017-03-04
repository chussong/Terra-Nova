#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <memory>
#include <iostream>

#include "person.hpp"
#include "colony.hpp"
#include "map.hpp"
#include "gamewindow.hpp"

class person;
class colony;
class gameWindow;
class map;
class game {
	std::vector<std::shared_ptr<person>> people;
	std::vector<std::shared_ptr<colony>> colonies;
	std::vector<std::shared_ptr<map>> maps;

	std::shared_ptr<gameWindow> win;

	public:
		game(){ win = std::make_shared<gameWindow>("Terra Nova", 100, 100, 1024, 768); }
		bool Tick();	// false means quit, true means continue ticking

		std::shared_ptr<person> CreatePerson();
		std::shared_ptr<colony> CreateColony(std::shared_ptr<map> parentMap,
				const int row, const int colm);
		std::shared_ptr<map> CreateMap();

		std::shared_ptr<gameWindow> Window() { return win; }

		void NextTurn();
};

#endif
