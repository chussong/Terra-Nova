#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <memory>
#include <iostream>

#include "person.hpp"
#include "colony.hpp"
#include "map.hpp"
#include "gamewindow.hpp"

class game {
	std::vector<std::shared_ptr<person>> people;
	std::vector<std::shared_ptr<colony>> colonies;
	std::vector<std::shared_ptr<map>> maps;

	public:
		bool Tick();	// false = quit

		std::shared_ptr<person> CreatePerson();
		std::shared_ptr<colony> CreateColony(std::shared_ptr<map> parentMap,
				const int row, const int colm);
		std::shared_ptr<map> CreateMap();

		void NextTurn(gameWindow* win);
};

#endif
